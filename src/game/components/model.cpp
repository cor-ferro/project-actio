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

        Model::Model(Model::File& modelFile) : Model()
        {
            console::info("load from file %s", modelFile.name);
            Assimp::Importer importer;

            std::string pFile = modelFile.file.getPath();
            unsigned int flags = aiProcessPreset_TargetRealtime_Quality | aiProcess_GenSmoothNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace;
//
            if (modelFile.flipUv) {
                flags|= aiProcess_FlipUVs;
            }

            const aiScene * scene = importer.ReadFile(pFile, flags);
            const Resource::Assimp * assimpResource = new Resource::Assimp(scene, pFile);

            if(!scene)
            {
                console::err("failed model loading %s", importer.GetErrorString());
                return;
            }

            initFromAi(assimpResource);

            setName(modelFile.name);

            importer.FreeScene();
            delete assimpResource;
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
            for (auto node : nodes)
                queueNodes.push(node);

            std::queue<std::string> queueImages;
            for (auto path : texturePaths)
                queueImages.push(path);

            assert(queueNodes.front() == assimpResource->getRootNode());

            rootNode_ = new Model::Node(queueNodes.front());
            queueNodes.pop();

            std::mutex lock;
            auto nodeLoader = [this, &lock, &assimpResource](std::queue<aiNode *> &queue) {
                aiNode *node = nullptr;
                while (queue.size() > 0) {
                    lock.lock();
                    node = queue.front();
                    queue.pop();
                    lock.unlock();
//			console::info(std::this_thread::get_id());

                    Model::Node *modelNode = new Model::Node(node);
                    this->addNode(modelNode); // todo: atomic insert

                    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
                        unsigned int meshIndex = node->mMeshes[i];
                        const aiMesh *mesh = assimpResource->getMesh(meshIndex);
                        const aiMaterial *meshMaterial = assimpResource->getMeshMaterial(mesh);

                        Mesh *modelMesh = Mesh::Create();
                        modelMesh->material.initFromAi(meshMaterial, assimpResource, images_);
                        modelMesh->geometry.initFromAi(mesh, assimpResource);

                        modelMesh->boneTransforms.reserve(mesh->mNumBones);
                        modelMesh->boneTransforms.resize(mesh->mNumBones);

//                        if (mesh->mNumBones > 0) {
//                            std::vector<BoneMap> boneJointsMap;
//                            ozz::Range<const char* const> jointNames = skeleton->joint_names();
//
//                            for (uint boneId = 0; boneId < mesh->mNumBones; boneId++) {
//                                aiBone * bone = mesh->mBones[boneId];
//                                const char* boneName = bone->mName.C_Str();
//                                uint jointIndex = 0;
//
//                                for (uint jIndex = 0; jIndex < jointNames.Count(); jIndex++) {
//                                    if (strcmp(jointNames[jIndex], boneName) == 0) {
//                                        jointIndex = jIndex;
//                                        break;
//                                    }
//                                }
//
//                                BoneMap map;
//                                map.jointIndex = jointIndex;
//                                map.boneIndex = boneId;
//                                map.offset = libAi::toNativeType(bone->mOffsetMatrix);
//
//                                boneJointsMap.push_back(map);
//                            }
//
//                            boneMeshMap.insert({ modelMesh, boneJointsMap });
//                        }

                        this->addMesh(modelMesh);
                        modelNode->addMesh(modelMesh);
                    }

                    // todo: выстраивать иерархию нод
                }
            };

            auto imageLoader = [this, &lock, &assimpResource](std::queue<std::string> &queue) {
                std::string path("");
                while (queue.size() > 0) {
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

//            for (auto mesh = meshes_.begin(); mesh != meshes_.end(); mesh++) {
//                (*mesh)->setup();
//                (*mesh)->material.setupTextures();
//            }

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

        const int Model::getNodesCount() {
            return nodes_.size();
        }
    }
}