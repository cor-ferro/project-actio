//
// Created by demitriy on 4/5/18.
//

#ifndef ACTIO_ANIMATIONS_H
#define ACTIO_ANIMATIONS_H

#include <entityx/entityx/System.h>
#include <entityx/entityx/config.h>
#include <entityx/entityx/Entity.h>
#include <entityx/entityx/Event.h>
#include "../components/skin.h"
#include "../components/model.h"
#include "../events/key_press.h"
#include "../../lib/input_handler.h"
#include "../context.h"
#include "base.h"

namespace game {
    namespace systems {
        namespace ex = entityx;

        class Animations
                : public systems::BaseSystem
                  , public ex::System<Animations>
                  , public ex::Receiver<Animations> {
        public:
            explicit Animations(Context *context) : systems::BaseSystem(context) {}

            void configure(ex::EventManager &event_manager) {
                event_manager.subscribe<events::KeyPress>(*this);
            }

            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override {
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

            void receive(const events::KeyPress &event) {
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

        private:
            bool isChangeState = false;
            bool isMoving = false;
            bool isJump = false;
        };
    }
}

#endif //ACTIO_ANIMATIONS_H
