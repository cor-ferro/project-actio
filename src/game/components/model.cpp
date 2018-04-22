#include "model.h"
#include <algorithm>
#include <thread>
#include <mutex>
#include <queue>
#include <unordered_set>
#include "model.h"
#include "../../lib/console.h"
#include "../../lib/ozz.h"
#include "../../lib/assimp.h"
#include "../../math/Box3.h"

namespace game {
    namespace components {
        Model::File::File()
                : name("")
                , flipUv(false) {}

        Model::File::File(std::string filePath)
                : file(filePath) {}

        Model::File::File(const Model::File &other)
                : name(other.name)
                , file(other.file)
                , flipUv(other.flipUv)
                , animation(other.animation) {}

        Model::Node::Node() : name("") {}

        Model::Node::~Node() {}

        Model::Node::Node(aiNode *node) : name(std::string(node->mName.C_Str())) {
            meshes.reserve(node->mNumMeshes);
            children.reserve(node->mNumChildren);

            transformation = libAi::toNativeType(node->mTransformation);
        }

        void Model::Node::addMesh(Mesh *mesh) {
            meshes.push_back(mesh);
        }

        void Model::Node::addNode(Model::Node *node) {
            children.push_back(node);
        }

        Model::Model() : name_() {}

        Model::Model(Mesh *mesh) : Model() {
            addMesh(mesh);
        }

        Model::Model(Model::File &modelFile) : Model() {
            setName(modelFile.name);
        }

        Model::Model(const Resource::Assimp *assimpResource) {
            initFromAi(assimpResource);
        }

        Model::~Model() {
            destroy();
        }

        void Model::destroy() {
            freeMeshes();
            freeNodes();
        }

        const ModelName &Model::getName() {
            return name_;
        }

        void Model::setName(std::string newName) {
            name_ = newName;
        }

        void Model::freeMeshes() {
            for (auto mesh : meshes_) {
                Mesh::Destroy(mesh);
            }

            meshes_.clear();
        }

        void Model::freeNodes() {
            nodes_.clear();
        }

        void Model::initFromAi(const Resource::Assimp *assimpResource) {
            freeMeshes();

            std::unordered_set<std::string> texturePaths = assimpResource->getTexturePaths();
            std::vector<aiNode *> nodes = assimpResource->getAllNodes();

            console::info("nodes.size(): %i", nodes.size());

            std::queue<aiNode *> queueNodes;
            for (auto node : nodes) queueNodes.push(node);

            std::queue<std::string> queueImages;
            for (auto path : texturePaths)
                queueImages.push(path);

            assert(queueNodes.front() == assimpResource->getRootNode());

            rootNode_ = new Model::Node(queueNodes.front());
            queueNodes.pop();

            std::mutex lock;
            auto nodeLoader = [this, &lock, &assimpResource](std::queue<aiNode *> &queue) {
                aiNode *node = nullptr;

                while (!queue.empty()) {
                    lock.lock();
                    node = queue.front();
                    queue.pop();
                    lock.unlock();

                    auto *modelNode = new Model::Node(node);
                    this->addNode(modelNode); // todo: atomic insert

                    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
                        unsigned int meshIndex = node->mMeshes[i];
                        const aiMesh *mesh = assimpResource->getMesh(meshIndex);
                        const aiMaterial *meshMaterial = assimpResource->getMeshMaterial(mesh);

                        Mesh *modelMesh = Mesh::Create();
                        modelMesh->setName(mesh->mName.C_Str());
                        modelMesh->material.initFromAi(meshMaterial, assimpResource, images_);
                        modelMesh->geometry.initFromAi(mesh, assimpResource);

                        if (mesh->mNumBones > 0) {
                            modelMesh->bones.resize(mesh->mNumBones);

                            for (uint boneId = 0; boneId < mesh->mNumBones; boneId++) {
                                aiBone *bone = mesh->mBones[boneId];
                                modelMesh->bones.offsets[boneId] = libAi::toNativeType(bone->mOffsetMatrix);
                                modelMesh->bones.names[boneId] = std::string(bone->mName.C_Str());
                            }
                        }

                        this->addMesh(modelMesh);
                        modelNode->addMesh(modelMesh);
                    }

                    // todo: выстраивать иерархию нод
                }
            };

            auto imageLoader = [this, &lock, &assimpResource](std::queue<std::string> &queue) {
                std::string path("");
                while (!queue.empty()) {
                    lock.lock();
                    path = queue.front();
                    queue.pop();
                    lock.unlock();

                    ImageLoader::Data imageData = ImageLoader::load(path);

                    if (!imageData.isReady()) {
                        imageData = ImageLoader::load(assimpResource->getDefaultTexturePath());
                    }

                    this->images_.insert({path, imageData});
                }
            };

            const uint countThreads = 1;
            std::vector<std::thread> nodeThreads;
            std::vector<std::thread> imageThreads;

            console::info("start load images");
            for (uint i = 0; i < countThreads; i++)
                imageThreads.push_back(std::thread(imageLoader, std::ref(queueImages)));
            for (uint i = 0; i < countThreads; i++)
                imageThreads[i].join();
            console::info("end load images");

            console::info("start init meshes");
            for (uint i = 0; i < countThreads; i++)
                nodeThreads.push_back(std::thread(nodeLoader, std::ref(queueNodes)));
            for (uint i = 0; i < countThreads; i++)
                nodeThreads[i].join();
            console::info("end init meshes %i", nodes_.size());
        }

        void Model::addMesh(Mesh *mesh) {
            meshes_.push_back(mesh);
        }

        void Model::addNode(Node *node) {
            nodes_.insert({node->name, std::shared_ptr<Node>(node)});
        }

        void Model::removeMesh(Mesh *mesh) {
            ModelMeshes::iterator it = std::find_if(meshes_.begin(), meshes_.end(), [&mesh](const Mesh *ptr) {
                return ptr == mesh;
            });

            if (it != meshes_.end()) {
                meshes_.erase(it);
            }
        }

        void Model::removeNode(Node *node) {
            auto it = nodes_.find(node->name);

            if (it != nodes_.end()) {
                nodes_.erase(it);
            }
        }

        const ModelMeshes &Model::getMeshes() {
            return meshes_;
        }

        Mesh *Model::getFirstMesh() {
            return meshes_.at(0);
        }

        Mesh *Model::getMesh() {
            return meshes_.at(0);
        }

        const int Model::getNodesCount() {
            return nodes_.size();
        }

        float Model::height() {
            float maxHeight = 0.0f;

            for (auto mesh : meshes_) {
                float geometryHeight = mesh->geometry.height();

                if (geometryHeight > maxHeight) {
                    maxHeight = geometryHeight;
                }
            }

            return maxHeight;
        }

        float Model::halfHeight() {
            float maxHeight = height();

            if (maxHeight > 0.0f) {
                return maxHeight / 2.0f;
            }

            return 0.0f;
        }

        Math::Box3 Model::getBoundingBox() {
            Math::Box3 maxBox;

            for (auto mesh : meshes_) {
                const Math::Box3 &bBox = mesh->geometry.getBoundingBox();

                maxBox.radius.x = glm::max(bBox.radius.x, maxBox.radius.x);
                maxBox.radius.y = glm::max(bBox.radius.y, maxBox.radius.y);
                maxBox.radius.z = glm::max(bBox.radius.z, maxBox.radius.z);
            }

            return maxBox;
        }

        void Model::reindexMeshBones(std::unordered_map<std::string, uint> &nodeIndexes) {
            // fill mesh node->bone indexes
            for (auto &mesh : meshes_) {
                for (unsigned int boneId = 0; boneId < mesh->bones.count; boneId++) {
                    auto nodeIndexIt = nodeIndexes.find(mesh->bones.names[boneId]);

                    if (nodeIndexIt != nodeIndexes.end()) {
                        mesh->bones.indexes[boneId] = nodeIndexIt->second;
                    } else {
                        console::warn("node not found: %s", mesh->bones.names[boneId]);
                        mesh->bones.indexes[boneId] = 0;
                    }
                }
            }
        }
    }
}