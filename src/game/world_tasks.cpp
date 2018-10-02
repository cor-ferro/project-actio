#include "world_tasks.h"
#include "world.h"
#include "../core/geometry_builder.h"
#include "../core/material_builder.h"
#include "../lib/assets_model.h"

namespace game {
    WorldTask::WorldTask(WorldTaskContext *context) : context(context) {}

    WorldTask::~WorldTask() {
        if (context != nullptr) {
            delete context;
            context = nullptr;
        }
    }

    WorldTaskContext::WorldTaskContext(World *world) : world(world) {}

    WorldTaskContext::~WorldTaskContext() {
        world = nullptr;
    }

    void *WorldTaskContext::getData() {
        return data;
    }

    void WorldTaskContext::setData(void *data) {
        WorldTaskContext::data = data;
    }

    World *WorldTaskContext::getWorld() {
        return world;
    }

    WorldModelLoadTask::WorldModelLoadTask(WorldTaskContext *context) : WorldTask(context) {

    }

    void WorldModelLoadTask::onStart() {
        assert(context != nullptr);

        void *data = context->getData();
        auto *asset = reinterpret_cast<assets::Model *>(data);

        if (asset == nullptr) {
            return;
        }

        asset->load();

        const aiScene *scene = asset->getScene();
        World *world = context->getWorld();

        if (scene != nullptr) {
            const ::Resource::Assimp *assimpResource = asset->getAiResource();

            const unsigned int numMeshes = scene->mNumMeshes;
            for (unsigned int i = 0; i < numMeshes; i++) {
                aiMesh *mesh = scene->mMeshes[i];
                aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

                std::shared_ptr<Mesh> modelMesh = Mesh::Create();

                modelMesh->setName(mesh->mName.C_Str());

                GeometryBuilder::FromAi(modelMesh->geometry, mesh);
                MaterialBuilder::FromAi(modelMesh->material, material, assimpResource, world->getAssets().get());

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

    void WorldModelLoadTask::onFinish() {

    }

    void WorldModelLoadTask::onFlush() {
        assert(context != nullptr);

        void *data = context->getData();
        auto *asset = reinterpret_cast<assets::Model *>(data);

        // trigger asset load in the app thread
        asset->onLoad();
    }
}