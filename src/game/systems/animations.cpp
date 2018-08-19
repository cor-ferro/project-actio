#include "animations.h"
#include "../world.h"
#include "../components/meshes.h"

namespace game {
    namespace systems {
        Animations::Animations(game::World *world) : systems::BaseSystem(world) {}

        void Animations::configure(ex::EventManager &event_manager) {}

        void Animations::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
            es.each<c::Meshes, c::Skin>([&dt](ex::Entity, c::Meshes &meshes, c::Skin &skin) {
                if (skin.canProcessAnimation()) {
                    skin.processAnimation(dt);
                    skin.animSamplers.update(dt);

                    mat4 *transforms = skin.getTransforms();

                    for (auto &mesh : meshes.items()) {
                        for (unsigned int boneIndex = 0; boneIndex < mesh->bones.count; boneIndex++) {
                            const unsigned int &nodeIndex = mesh->bones.indexes[boneIndex];

                            mesh->bones.transforms[boneIndex] = transforms[nodeIndex];
                        }
                    }
                }
            });
        }
    }
}