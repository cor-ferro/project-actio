#include "tasks.h"
#include "../core/geometry_builder.h"
#include "../core/material_builder.h"
#include "../lib/assets_model.h"

namespace game {
    Task::Task(TaskContext& taskContext) : m_taskContext(taskContext) {}

    TaskContext::TaskContext(Context& context) : m_context(context) {}

    void *TaskContext::getData() {
        return data;
    }

    void TaskContext::setData(void *data) {
        TaskContext::data = data;
    }

    Context& TaskContext::getContext() {
        return m_context;
    }

    TaskLoadModel::TaskLoadModel(TaskContext& taskContext) : Task(taskContext) {}

    void TaskLoadModel::onStart() {
        assert(context != nullptr);

        void *data = m_taskContext.getData();
        auto *asset = reinterpret_cast<assets::Model *>(data);

        if (asset == nullptr) {
            return;
        }

        asset->load();

        const aiScene *scene = asset->getScene();
        Context& context = m_taskContext.getContext();

        if (scene != nullptr) {
            const ::resources::Assimp *assimpResource = asset->getAiResource();

            const unsigned int numMeshes = scene->mNumMeshes;
            for (unsigned int i = 0; i < numMeshes; i++) {
                aiMesh *mesh = scene->mMeshes[i];
                aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

                MeshHandle modelMesh = context.meshes().create();

                modelMesh->setName(mesh->mName.C_Str());

                GeometryBuilder::FromAi(modelMesh->geometry, mesh);
                MaterialBuilder::FromAi(modelMesh->material, material, assimpResource, context.getAssets().get());

                if (mesh->mNumBones > 0) {
                    modelMesh->bones.resize(mesh->mNumBones);

                    for (uint boneId = 0; boneId < mesh->mNumBones; boneId++) {
                        aiBone *bone = mesh->mBones[boneId];
                        modelMesh->bones.offsets[boneId] = libAi::toNativeType(bone->mOffsetMatrix);
                        modelMesh->bones.names[boneId] = std::string(bone->mName.C_Str());
                    }
                }

                asset->addMesh(modelMesh);
            }
        }

        asset->markLoaded();
    }

    void TaskLoadModel::onFinish() {

    }

    void TaskLoadModel::onFlush() {
        assert(context != nullptr);

        void *data = m_taskContext.getData();
        auto *asset = reinterpret_cast<assets::Model *>(data);

        // trigger asset load in the app thread
        asset->onLoad();
    }
}