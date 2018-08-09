#include "animations.h"
#include "../world.h"

namespace game {
    namespace systems {
        Animations::Animations(game::World *world) : systems::BaseSystem(world) {}

        void Animations::configure(ex::EventManager &event_manager) {
            event_manager.subscribe<events::KeyPress>(*this);
        }

        void Animations::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
            ex::ComponentHandle<components::Model> model;
            ex::ComponentHandle<components::Skin> skin;

            for (ex::Entity entity : es.entities_with_components(model, skin)) {
                if (skin->canProcessAnimation()) {
                    skin->processAnimation(dt);
                    skin->animSamplers.update(dt);

                    mat4 *transforms = skin->getTransforms();

                    for (Mesh *mesh : model->getMeshes()) {
                        for (unsigned int boneIndex = 0; boneIndex < mesh->bones.count; boneIndex++) {
                            const unsigned int &nodeIndex = mesh->bones.indexes[boneIndex];

                            mesh->bones.transforms[boneIndex] = transforms[nodeIndex];
                        }
                    }
                }
            }
        }

        void Animations::receive(const events::KeyPress &event) {
            bool isMovingKey = event.key == InputHandler::KEY_W
                               || event.key == InputHandler::KEY_S
                               || event.key == InputHandler::KEY_A
                               || event.key == InputHandler::KEY_D;

            if (isMovingKey) {
                isMoving = event.action != 0;
                isChangeState = true;
            }

            if (event.key == InputHandler::KEY_SPACE) {
                isJump = true;
                isChangeState = true;
            }
        }
    }
}