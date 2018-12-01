#include "model_builder.h"
#include <unordered_set>
#include <queue>
#include <thread>
#include <mutex>
#include <boost/lockfree/stack.hpp>
#include <boost/lockfree/queue.hpp>
#include "../lib/assimp.h"
#include "geometry_builder.h"
#include "material_builder.h"

void ModelBuilder::FromAi(Model *model, resources::Assimp *assimpResource, Assets *assets)  {
//    model->freeMeshes();
//
//    boost::lockfree::stack<Model::Node*> setupNodes(128);
//    boost::lockfree::queue<const aiNode*> queueNodes(128);
//
//    std::function<void(const aiNode *)> collectNodes;
//    collectNodes = [&collectNodes, &queueNodes](const aiNode *parentNode)->void {
//        queueNodes.push(parentNode);
//
//        for (unsigned int i = 0; i < parentNode->mNumChildren; i++) {
//            aiNode * childNode = parentNode->mChildren[i];
//            collectNodes(childNode);
//        }
//    };
//
//    collectNodes(assimpResource->getRootNode());
//
//    aiNode *aiRootNode = nullptr;
//    queueNodes.pop(aiRootNode);
//
//    assert(aiRootNode != nullptr);
//    assert(aiRootNode == assimpResource->getRootNode());
//    model->rootNode_ = new Model::Node(aiRootNode);
//
//    auto nodeLoader = [&model, &assets, &assimpResource, &setupNodes](boost::lockfree::queue<const aiNode*> &queue) {
//        aiNode *node = nullptr;
//
//        while (queue.pop(node)) {
//            assert(node != nullptr);
//
//            auto *modelNode = new Model::Node(node);
//
//            for (unsigned int i = 0; i < node->mNumMeshes; i++) {
//                unsigned int meshIndex = node->mMeshes[i];
//                const aiMesh *mesh = assimpResource->getMesh(meshIndex);
//                const aiMaterial *meshMaterial = assimpResource->getMeshMaterial(mesh);
//
//                std::shared_ptr<Mesh> modelMesh = Mesh::Create();
//                modelMesh->setName(mesh->mName.C_Str());
//
//                GeometryBuilder::FromAi(modelMesh->geometry, mesh);
//                MaterialBuilder::FromAi(modelMesh->material, meshMaterial, assimpResource, assets);
//
//                if (mesh->mNumBones > 0) {
//                    modelMesh->bones.resize(mesh->mNumBones);
//
//                    for (uint boneId = 0; boneId < mesh->mNumBones; boneId++) {
//                        aiBone *bone = mesh->mBones[boneId];
//                        modelMesh->bones.offsets[boneId] = libAi::toNativeType(bone->mOffsetMatrix);
//                        modelMesh->bones.names[boneId] = std::string(bone->mName.C_Str());
//                    }
//                }
//
//                modelNode->add(modelMesh);
//            }
//
//            setupNodes.push(modelNode);
//        }
//    };
//
//    const uint countThreads = 1;
//    std::vector<std::thread> nodeThreads;
//
//    for (uint i = 0; i < countThreads; i++)
//        nodeThreads.push_back(std::thread(nodeLoader, std::ref(queueNodes)));
//    for (uint i = 0; i < countThreads; i++)
//        nodeThreads[i].join();
//
//    Model::Node* setupNode;
//    while (setupNodes.pop(setupNode)) {
//        model->add(setupNode);
//
//        for (auto &mesh : setupNode->meshes) {
//            model->add(mesh);
//        }
//    }
}