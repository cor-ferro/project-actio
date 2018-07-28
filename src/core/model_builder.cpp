#include "model_builder.h"
#include <unordered_set>
#include <queue>
#include <thread>
#include <mutex>
#include <boost/lockfree/stack.hpp>
#include <boost/lockfree/queue.hpp>
#include "../lib/assimp.h"

void ModelBuilder::FromAi(Model *model, Resource::Assimp *assimpResource, Assets *assets)  {
    model->freeMeshes();

    std::unordered_set<std::string> texturePaths = assimpResource->getTexturePaths();
    std::vector<aiNode *> nodes = assimpResource->getAllNodes();

    console::info("nodes.size(): %i", nodes.size());

    boost::lockfree::stack<Model::Node*> setupNodes(128);
    boost::lockfree::queue<aiNode*> queueNodes(128);

    for (auto node : nodes) queueNodes.push(node);

    std::queue<std::string> queueImages;
    for (const auto &path : texturePaths)
        queueImages.push(path);

    aiNode *aiRootNode = nullptr;
    queueNodes.pop(aiRootNode);

    assert(aiRootNode != nullptr);
    assert(aiRootNode == assimpResource->getRootNode());
    model->rootNode_ = new Model::Node(aiRootNode);

    auto nodeLoader = [&model, &assets, &assimpResource, &setupNodes](boost::lockfree::queue<aiNode*> &queue) {
        aiNode *node = nullptr;

        while (queue.pop(node)) {
            assert(node != nullptr);

            auto *modelNode = new Model::Node(node);

            for (unsigned int i = 0; i < node->mNumMeshes; i++) {
                unsigned int meshIndex = node->mMeshes[i];
                const aiMesh *mesh = assimpResource->getMesh(meshIndex);
                const aiMaterial *meshMaterial = assimpResource->getMeshMaterial(mesh);

                std::shared_ptr<Material> material = std::make_shared<Material>(libAi::createMaterial(meshMaterial, assimpResource, assets));

                Mesh *modelMesh = Mesh::Create(material);
                modelMesh->setName(mesh->mName.C_Str());
                modelMesh->geometry.initFromAi(mesh, assimpResource);

                if (mesh->mNumBones > 0) {
                    modelMesh->bones.resize(mesh->mNumBones);

                    for (uint boneId = 0; boneId < mesh->mNumBones; boneId++) {
                        aiBone *bone = mesh->mBones[boneId];
                        modelMesh->bones.offsets[boneId] = libAi::toNativeType(bone->mOffsetMatrix);
                        modelMesh->bones.names[boneId] = std::string(bone->mName.C_Str());
                    }
                }

                modelNode->addMesh(modelMesh);
            }

            setupNodes.push(modelNode);
        }
    };

    const uint countThreads = 4;
    std::vector<std::thread> nodeThreads;

    console::info("start init meshes");

    for (uint i = 0; i < countThreads; i++)
        nodeThreads.push_back(std::thread(nodeLoader, std::ref(queueNodes)));
    for (uint i = 0; i < countThreads; i++)
        nodeThreads[i].join();

    Model::Node* setupNode;
    while (setupNodes.pop(setupNode)) {
        model->addNode(setupNode);

        for (auto &mesh : setupNode->meshes) {
            model->addMesh(mesh);
        }
    }

    console::info("end init meshes %i", model->nodes_.size());
}