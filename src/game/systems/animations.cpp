#include "animations.h"
#include "../components/mesh.h"

namespace game {
    namespace systems {
        Animations::Animations(Context& context) : systems::BaseSystem(context) {}

        void Animations::configure(ex::EventManager &event_manager) {}

        void Animations::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
            es.each<c::Mesh, c::Skin>([&dt](ex::Entity, c::Mesh &meshes, c::Skin &skin) {
                if (skin.canProcessAnimation()) {
                    skin.processAnimation(dt);
                    skin.animSamplers.update(dt);

                    mat4 *transforms = skin.getTransforms();

                    for (auto &mesh : meshes.getAll()) {
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