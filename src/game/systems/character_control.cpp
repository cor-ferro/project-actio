#include "character_control.h"
#include "../world.h"
#include "../../lib/utils.h"

namespace game {
    namespace systems {
        CharacterControl::CharacterControl(World *world) : systems::BaseSystem(world) {}

        void CharacterControl::configure(ex::EventManager &event_manager) {
            event_manager.subscribe<events::KeyPress>(*this);
            event_manager.subscribe<events::SetupControlled>(*this);
        }

        void CharacterControl::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
            ex::ComponentHandle<components::Model> model;
            ex::ComponentHandle<components::Skin> skin;
            ex::ComponentHandle<components::Controlled> control;
            ex::ComponentHandle<components::Transform> transform;

//                console::info("press: %i %i", isLeftPress, isRightPress);

            for (ex::Entity entity : es.entities_with_components(model, skin, control, transform)) {
                const float maxSpeed = 1.5f;
                const float minSpeed = -1.5f;
                const float jumpForce = 1.2f;
                const float speedFactor = 0.1f;

                const float minDirectionAngle = 0.0f;
                const float maxDirectionAngle = 180.0f;
                const float anglePerTick = static_cast<float>(dt);
                const float increaseValue = 0.3f;
                const float reduceValue = 0.1f;

                if (!isLeftPress && !isRightPress) {
                    control->delta.z = 0.0f;
                }

                if (isLeftPress) {
                    control->deltaX.z = glm::clamp(control->deltaX.z - increaseValue, minSpeed, maxSpeed);
                    control->sideDirection = components::Controlled::Left;
                }

                if (isRightPress) {
                    control->deltaX.z = glm::clamp(control->deltaX.z + increaseValue, minSpeed, maxSpeed);
                    control->sideDirection = components::Controlled::Right;
                }

                switch (control->sideDirection) {
                    case components::Controlled::Left:
                        control->angle = glm::clamp(control->angle + anglePerTick, minDirectionAngle, maxDirectionAngle);
                        break;
                    case components::Controlled::Right:
                        control->angle = glm::clamp(control->angle - anglePerTick, minDirectionAngle, maxDirectionAngle);
                        break;
                }

                if (control->angle != minDirectionAngle || control->angle != maxDirectionAngle) {
                    transform->setQuaternion(
                            glm::sin(0.0f),
                            glm::sin(glm::radians(control->angle) / 2.0f),
                            glm::sin(0.0f),
                            glm::cos(glm::radians(control->angle) / 2.0f)
                    );
                }


                if (isMovingKeyPress || isJumpPress) {
                    if (isJumpPress && !control->isJump) {
                        control->jumpForce = jumpForce;
                        control->isJump = true;
                        isJumpPress = false;
                    }
                } else {


                    control->deltaX.x = control->deltaX.x > 0.0f
                                        ? glm::clamp(control->deltaX.x - reduceValue, 0.0f, maxSpeed)
                                        : glm::clamp(control->deltaX.x + reduceValue, minSpeed, 0.0f);

                    control->deltaX.z = control->deltaX.z > 0.0f
                                        ? glm::clamp(control->deltaX.z - reduceValue, 0.0f, maxSpeed)
                                        : glm::clamp(control->deltaX.z + reduceValue, minSpeed, 0.0f);
                }

                float &x = control->deltaX.x;
                float &z = control->deltaX.z;

                control->delta.x = (glm::pow(fE, x * 1.7f) - glm::pow(fE, x * -0.3f)) / (glm::pow(fE, x) * 1.0f);
                control->delta.z = (glm::pow(fE, z * 1.7f) - glm::pow(fE, z * -0.3f)) / (glm::pow(fE, z) * 1.0f);

                control->delta.x = glm::clamp(control->delta.x, minSpeed, maxSpeed);
                control->delta.z = glm::clamp(control->delta.z, minSpeed, maxSpeed);

                control->disp.x = speedFactor * control->delta.x;
                control->disp.z = speedFactor * control->delta.z;

                float speed = (glm::abs(control->delta.x) + glm::abs(control->delta.z));
                float scaleSpeed = utils::scaleNumber(0.0f, maxSpeed, speed);

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

        void CharacterControl::receive(const events::KeyPress &event) {
            bool isMovingKey = event.key == InputHandler::KEY_W ||
                               event.key == InputHandler::KEY_S ||
                               event.key == InputHandler::KEY_A ||
                               event.key == InputHandler::KEY_D;

            if (isMovingKey) {
                isMovingKeyPress =
                        event.action == InputHandler::KEY_PRESS || event.action == InputHandler::KEY_REPEAT;
            }


            bool isJumpKey = event.key == InputHandler::KEY_SPACE;

            if (event.key == InputHandler::KEY_W) {
                isForwardPress = event.action != InputHandler::KEY_RELEASE;
            }

            if (event.key == InputHandler::KEY_S) {
                isBackwardPress = event.action != InputHandler::KEY_RELEASE;
            }

            if (event.key == InputHandler::KEY_A) {
                if (event.action == InputHandler::KEY_PRESS) {
                    isLeftPress = true;
                    isRightPress = false;
                } else if (event.action == InputHandler::KEY_RELEASE) {
                    isLeftPress = false;
                }
            }

            if (event.key == InputHandler::KEY_D) {

                if (event.action == InputHandler::KEY_PRESS) {
                    isRightPress = true;
                    isLeftPress = false;
                } else if (event.action == InputHandler::KEY_RELEASE) {
                    isRightPress = false;
                }
            }

            if (isJumpKey && event.action == InputHandler::KEY_PRESS) {
                isJumpPress = true;
            }
        }

        void CharacterControl::receive(const events::SetupControlled &event) {
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
    }
}