//
// Created by demitriy on 4/11/18.
//

#ifndef ACTIO_CHARACTER_CONTROL_H
#define ACTIO_CHARACTER_CONTROL_H

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

namespace game {
    namespace systems {
        using namespace entityx;

        class CharacterControl : public entityx::System<CharacterControl>, public entityx::Receiver<CharacterControl> {
        public:
            void configure(EventManager &event_manager) {
                event_manager.subscribe<events::KeyPress>(*this);
                event_manager.subscribe<events::SetupControlled>(*this);
            }

            void update(EntityManager &es, EventManager &events, TimeDelta dt) override {
                ComponentHandle<components::Model> model;
                ComponentHandle<components::Skin> skin;
                ComponentHandle<components::Controlled> control;

                for (Entity entity : es.entities_with_components(model, skin, control)) {
                    const float maxSpeed = 1.5f;
                    const float minSpeed = -0.7f;
                    const float jumpForce = 1.2f;
                    const float speedFactor = 0.1f;

                    if (isMovingKeyPress || isJumpPress) {
                        const float increaseValue = 0.03f;

//                        if (isForwardPress) control->deltaX.x+= increaseValue;
//                        if (isBackwardPress) control->deltaX.x-= increaseValue;
                        if (isLeftPress) control->deltaX.z -= increaseValue;
                        if (isRightPress) control->deltaX.z += increaseValue;

                        if (isJumpPress && !control->isJump) {
                            control->jumpForce = jumpForce;
                            control->isJump = true;
                            isJumpPress = false;
                        }
                    } else {
                        const float reduceValue = 0.05f;

                        control->deltaX.x = control->deltaX.x > 0.0f
                                ? glm::clamp(control->deltaX.x - reduceValue, 0.0f, maxSpeed)
                                : glm::clamp(control->deltaX.x + reduceValue, minSpeed, 0.0f);

                        control->deltaX.z = control->deltaX.z > 0.0f
                                ? glm::clamp(control->deltaX.z - reduceValue, 0.0f, maxSpeed)
                                : glm::clamp(control->deltaX.z + reduceValue, minSpeed, 0.0f);
                    }

                    float &x = control->deltaX.x;
                    float &z = control->deltaX.z;

                    control->delta.x = (glm::pow(fE, x * 2.5f) - glm::pow(fE, x * -1.5f)) / (glm::pow(fE, x) * 1.0f);
                    control->delta.z = (glm::pow(fE, z * 2.5f) - glm::pow(fE, z * -1.5f)) / (glm::pow(fE, z) * 1.0f);

                    control->delta.x = glm::clamp(control->delta.x, minSpeed, maxSpeed);
                    control->delta.z = glm::clamp(control->delta.z, minSpeed, maxSpeed);

                    control->disp.x = speedFactor * control->delta.x;
                    control->disp.z = speedFactor * control->delta.z;

                    float speed = (glm::abs(control->delta.x) + glm::abs(control->delta.z));
                    float scaleSpeed = math::scale(0.0f, maxSpeed, speed);

                    float idleAnimationWeight = 1.0f - scaleSpeed;
                    float runAnimationWeight = scaleSpeed;
                    float jumpAnimationWeight = 0.0f;

                    skin->animSamplers.setWeight(idleIndex, idleAnimationWeight);
                    skin->animSamplers.setWeight(runIndex, runAnimationWeight);
                    skin->animSamplers.setWeight(jumpIndex, jumpAnimationWeight);
                }

                if (isControlChange) {
                    isControlChange = false;
                }
            }

            void receive(const events::KeyPress &event) {
                bool isMovingKey = event.key == InputHandler::KEY_W ||
                                   event.key == InputHandler::KEY_S ||
                                   event.key == InputHandler::KEY_A ||
                                   event.key == InputHandler::KEY_D;

                if (isMovingKey) {
                    isMovingKeyPress = event.action == InputHandler::KEY_PRESS || event.action == InputHandler::KEY_REPEAT;
                }


                bool isJumpKey = event.key == InputHandler::KEY_SPACE;

                if (event.key == InputHandler::KEY_W) {
                    isForwardPress = event.action != InputHandler::KEY_RELEASE;
                }

                if (event.key == InputHandler::KEY_S) {
                    isBackwardPress = event.action != InputHandler::KEY_RELEASE;
                }

                if (event.key == InputHandler::KEY_A) {
                    isLeftPress = event.action != InputHandler::KEY_RELEASE;
                }

                if (event.key == InputHandler::KEY_D) {
                    isRightPress = event.action != InputHandler::KEY_RELEASE;
                }

                if (isJumpKey && event.action == InputHandler::KEY_PRESS) {
                    isJumpPress = true;
                }
            }

            void receive(const events::SetupControlled &event) {
                entityx::Entity entity = event.entity;

                auto skin = components::get<components::Skin>(entity);

                if (skin) {
                    animation::Animation *idleAnimation = skin->getAnimation("idle");
                    animation::Animation *walkAnimation = skin->getAnimation("course");
                    animation::Animation *jumpAnimation = skin->getAnimation("saut_air_montant");
                    animation::Animation *jumpStartAnimation = skin->getAnimation("saut_impulsion");
                    animation::Animation *jumpEndAnimation = skin->getAnimation("saut_reception");

                    // saut_impulsion - начало прыжка
                    // saut_air_montant - процесс полета вверх
                    // saut_air_transistion_descente - переход на снижение
                    // saut_air_descente - снижение
                    // saut_reception - приземление

                    skin->animSamplers.set(0, idleAnimation);
                    skin->animSamplers.set(1, walkAnimation);
                    skin->animSamplers.set(2, jumpAnimation);
                    skin->animSamplers.set(3, jumpStartAnimation);
                    skin->animSamplers.set(4, jumpEndAnimation);

                    isControlChange = true;
                }

//                entity.assign<components::CharacterControl>();
            }

        private:
            struct AnimationCommand {
                int animIndex;
                float weight;
                float step;
            };

            bool isMovingKeyPress = false;
            bool isForwardPress = false;
            bool isBackwardPress = false;
            bool isLeftPress = false;
            bool isRightPress = false;
            bool isJumpPress = false;
            bool isCanJump = false;

            int idleIndex = 0;
            int runIndex = 1;
            int jumpIndex = 2;
            float step = 1.0f / (60.0f * 0.5f);

            bool isControlChange = false;
        };
    }
}

#endif //ACTIO_CHARACTER_CONTROL_H
