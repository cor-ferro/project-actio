//
// Created by demitriy on 4/11/18.
//

#ifndef ACTIO_CHAR_CONTROL_H
#define ACTIO_CHAR_CONTROL_H

#include <entityx/entityx/System.h>
#include <entityx/entityx/config.h>
#include <entityx/entityx/Event.h>
#include <entityx/entityx/Entity.h>
#include <stack>
#include "../events/mouse_press.h"
#include "../events/key_press.h"
#include "../../lib/input_handler.h"
#include "../components/skin.h"
#include "../components/model.h"
#include "../events/setup_controlled.h"
#include "../components/base.h"
#include "../../lib/math.h"
#include "../context.h"
#include "base.h"
#include "../components/transform.h"
#include "../components/character.h"
#include "../components/char_items.h"
#include "../components/user_control.h"
#include "../components/ai_control.h"

namespace game {
    namespace systems {
        namespace ex = entityx;
        namespace c = components;

        class CharControl
                : systems::BaseSystem
                  , public entityx::System<CharControl>
                  , public ex::Receiver<CharControl> {
        public:
            explicit CharControl(Context *context)
                    : systems::BaseSystem(context) {}

            void configure(ex::EntityManager &es, entityx::EventManager &events) {
                events.subscribe<events::MousePress>(*this);
                entityManager = &es;
            }

            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override {
                es.each<c::Character, c::CharItems, c::UserControl, c::Transform, c::Skin>([&dt](
                        ex::Entity entity,
                        c::Character &character,
                        c::CharItems &charItems,
                        c::UserControl &userControl,
                        c::Transform &transform,
                        c::Skin &skin
                ) {
                    const float speedFactor = 0.1f;
                    const float MIN_DIRECTION_ANGLE = 0.0f;
                    const float MAX_DIRECTION_ANGLE = 180.0f;
                    const float speedIncrease = 0.3f; // multiple by mass
                    const float speedReduce = 0.1f; // multiple by mass
                    const auto ANGLE_PER_TICK = static_cast<float>(dt);

                    const float minZAxis = character.walkSpeed * -1.0f;
                    const float maxZAxis = character.walkSpeed;


                    if (!userControl.isLeftPress() && !userControl.isRightPress()) {
                        character.motionDelta.z = 0.0f;
                    }

                    if (userControl.isLeftPress()) {
                        character.viewSide = c::Character::ViewSide::Left;
                        character.motionDeltaX.z -= speedIncrease;
                        character.motionDeltaX.z = glm::clamp(character.motionDeltaX.z, minZAxis, maxZAxis);
                    }

                    if (userControl.isRightPress()) {
                        character.viewSide = c::Character::ViewSide::Right;
                        character.motionDeltaX.z += speedIncrease;
                        character.motionDeltaX.z = glm::clamp(character.motionDeltaX.z, minZAxis, maxZAxis);
                    }

                    switch (character.viewSide) {
                        case c::Character::ViewSide::Left:
                            character.viewAngle += ANGLE_PER_TICK;
                            break;
                        case c::Character::ViewSide::Right:
                            character.viewAngle -= ANGLE_PER_TICK;
                            break;
                    }

                    character.viewAngle = glm::clamp(character.viewAngle, MIN_DIRECTION_ANGLE, MAX_DIRECTION_ANGLE);

                    if (character.viewAngle != MIN_DIRECTION_ANGLE || character.viewAngle != MAX_DIRECTION_ANGLE) {
                        float halfViewRad = glm::radians(character.viewAngle) / 2.0f;

                        transform.setQuaternion(
                                glm::sin(0.0f),
                                glm::sin(halfViewRad),
                                glm::sin(0.0f),
                                glm::cos(halfViewRad)
                        );
                    }

                    if (userControl.isJumpPress() && !character.isJump) {
                        character.jump = character.jumpForce;
                        character.isJump = true;
                    } else {
                        character.motionDeltaX.z = character.motionDeltaX.z > 0.0f
                                                   ? glm::clamp(character.motionDeltaX.z - speedReduce, 0.0f, maxZAxis)
                                                   : glm::clamp(character.motionDeltaX.z + speedReduce, minZAxis, 0.0f);
                    }

                    character.motionDelta.z = (glm::pow(fE, character.motionDeltaX.z * 1.7f) -
                                               glm::pow(fE, character.motionDeltaX.z * -0.3f)) /
                                              (glm::pow(fE, character.motionDeltaX.z) * 1.0f);

                    character.motionDelta.z = glm::clamp(character.motionDelta.z, minZAxis, maxZAxis);
                    character.motion.z = speedFactor * character.motionDelta.z;

                    float speed = (glm::abs(character.motionDelta.x) + glm::abs(character.motionDelta.z));
                    float scaleSpeed = math::scale(0.0f, maxZAxis, speed);

                    float idleAnimationWeight = 1.0f - scaleSpeed;
                    float runAnimationWeight = scaleSpeed;
                    float jumpAnimationWeight = 0.0f;

                    skin.animSamplers.setWeight(0, idleAnimationWeight);
                    skin.animSamplers.setWeight(1, runAnimationWeight);
                    skin.animSamplers.setWeight(2, jumpAnimationWeight);
                });
            }

            void receive(const events::MousePress &event) {
                if (event.button == InputHandler::MOUSE_BUTTON_LEFT && event.action == InputHandler::KEY_PRESS) {
                    vec3 mousePos = worldContext->mouseWorldPosition;

                    entityManager->each<c::Character, c::CharItems, c::UserControl>([&mousePos](
                            ex::Entity entity,
                            c::Character &character,
                            c::CharItems &charItems,
                            c::UserControl &userControl
                    ) {
                        charItems.execActiveItem(mousePos);
                    });
                }
            }

        private:
            void handleUserControl() {}

            void handleAiControl() {}

            ex::EntityManager *entityManager = nullptr;
        };
    }
}

#endif // ACTIO_CHAR_CONTROL_H
