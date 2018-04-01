#ifndef GAME_SYSTEMS_MOVEMENT_H_
#define GAME_SYSTEMS_MOVEMENT_H_

#include <entityx/entityx.h>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include "../events/key_press.h"
#include "../components/transform.h"
#include "../components/camera.h"
#include "../components/target.h"
#include "../../lib/console.h"
#include "../../lib/input_handler.h"
#include "../events/mouse_press.h"

namespace game {
    namespace systems {
        using namespace entityx;

        class Movement : public entityx::System<Movement> {
        public:
            enum Key {
                KEY_SPACE = InputHandler::KEY_SPACE,
                KEY_ENTER = InputHandler::KEY_ENTER,
                KEY_W = InputHandler::KEY_W,
                KEY_A = InputHandler::KEY_A,
                KEY_S = InputHandler::KEY_S,
                KEY_D = InputHandler::KEY_D,
                KEY_C = InputHandler::KEY_C,
                KEY_ESC = InputHandler::KEY_ESC
            };

            enum KeyAction {
                KEY_PRESS = InputHandler::KEY_PRESS,
                KEY_RELEASE = InputHandler::KEY_RELEASE,
                KEY_REPEAT = InputHandler::KEY_REPEAT
            };

            enum MouseButton {
                MOUSE_BUTTON_LEFT = InputHandler::MOUSE_BUTTON_LEFT,
                MOUSE_BUTTON_MIDDLE = InputHandler::MOUSE_BUTTON_MIDDLE,
                MOUSE_BUTTON_RIGHT = InputHandler::MOUSE_BUTTON_RIGHT
            };

            explicit Movement(InputHandler *ih) : ih_(ih) {

            }

            void configure(entityx::EventManager &event_manager) {
                ih_->subscribeKeyPress([&event_manager](int key, int scancode, int action, int mods) {
                    event_manager.emit<events::KeyPress>(key, scancode, action, mods);
                });

                ih_->subscribeMousePress([&event_manager](int button, int action, int mods) {
                    event_manager.emit<events::MousePress>(button, action, mods);
                });
            }

            void update(EntityManager &es, EventManager &events, TimeDelta dt) override {
                ComponentHandle<components::Camera> camera;
                ComponentHandle<components::Transform> cameraTransform;
                ComponentHandle<components::Target> cameraTarget;

                time += dt;

                for (Entity entity : es.entities_with_components(camera, cameraTransform, cameraTarget)) {
                    updateCamera2(camera, dt);
                }
            }

            void updateCamera1(ComponentHandle<components::Camera> camera) {
                float speedX = glm::abs(ih_->mouseMoved.x * ih_->sensetivity.speedFactor);
                float speedY = glm::abs(ih_->mouseMoved.y * ih_->sensetivity.speedFactor);

                float angleX = ih_->mouseMoved.y * 0.03f * -glm::pow(speedX, ih_->sensetivity.sensetivity);
                float angleY = ih_->mouseMoved.x * 0.03f * -glm::pow(speedY, ih_->sensetivity.sensetivity);

                if (angleY > 100.0f) {
                    console::info("over angle y %f %i", angleY, ih_->mouseMoved.x);
                    console::info("details %i %i %i", ih_->mouseMoved.x, ih_->mouseStart.x, ih_->mouse.x);
                    angleY = 10.0f;
                }

                if (angleX > 100.0f) {
                    console::info("over angle x %i", angleX);
                    console::info("details %i %i %i", ih_->mouseMoved.y, ih_->mouseStart.y, ih_->mouse.y);
                    angleX = 10.0f;
                }

                vec3 cameraPosition = camera->getPosition();
                vec3 cameraRotation = camera->getRotation(); // remove link rotation
                vec3 cameraUp = camera->getUp();
                vec3 front = vec3(0.0f);

                cameraRotation.x += angleX;
                cameraRotation.y += angleY;

                float radRotationX = glm::radians(cameraRotation.x);
                float radRotationY = glm::radians(cameraRotation.y);

                front.x = -glm::cos(radRotationX) * glm::sin(radRotationY);
                front.y = glm::sin(radRotationX);
                front.z = glm::cos(radRotationX) * glm::cos(radRotationY);
                front = glm::normalize(front);

                vec3 left = glm::normalize(glm::cross(front, cameraUp));

                if (ih_->isPress(InputHandler::KEY_W)) cameraPosition -= speed * front;
                if (ih_->isPress(InputHandler::KEY_S)) cameraPosition += speed * front;
                if (ih_->isPress(InputHandler::KEY_A)) cameraPosition += left * speed;
                if (ih_->isPress(InputHandler::KEY_D)) cameraPosition -= left * speed;
                if (ih_->isPress(InputHandler::KEY_C)) cameraPosition -= vec3(0.0f, 0.1f, 0.0f);
                if (ih_->isPress(InputHandler::KEY_SPACE)) cameraPosition += vec3(0.0f, 0.1f, 0.0f);

                camera->setRotation(cameraRotation);
                camera->setPosition(cameraPosition);

//                console::info("%f, %f, %f", camera->getFront().x, camera->getFront().y, camera->getFront().z);
            }

            void updateCamera2(ComponentHandle<components::Camera> camera, TimeDelta dt) {
                yaw += ih_->mouseMoved.x * -0.03f;
                pitch += ih_->mouseMoved.y * 0.03f;

                if (pitch > 89.0f)
                    pitch = 89.0f;
                if (pitch < -89.0f)
                    pitch = -89.0f;

                vec3 cameraPosition = camera->getPosition();
                vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

                camera->target_.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                camera->target_.y = sin(glm::radians(pitch));
                camera->target_.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
                camera->target_ = glm::normalize(camera->target_);

                vec3 forwardDirection = camera->target_ * speed;
                vec3 sideDirection = glm::normalize(glm::cross(camera->target_, cameraUp)) * speed;

                if (ih_->isPress(InputHandler::KEY_W)) cameraPosition += forwardDirection;
                if (ih_->isPress(InputHandler::KEY_S)) cameraPosition -= forwardDirection;
                if (ih_->isPress(InputHandler::KEY_A)) cameraPosition -= sideDirection;
                if (ih_->isPress(InputHandler::KEY_D)) cameraPosition += sideDirection;
                if (ih_->isPress(InputHandler::KEY_C)) cameraPosition -= vec3(0.0f, 0.1f, 0.0f);
                if (ih_->isPress(InputHandler::KEY_SPACE)) cameraPosition += vec3(0.0f, 0.1f, 0.0f);

                camera->position_ = cameraPosition;
                camera->view_ = glm::lookAt(cameraPosition, cameraPosition + camera->target_, cameraUp);
            }

        private:
            InputHandler *ih_;

            float yaw = 0.0f;
            float pitch = 0.0f;

            double time = 0.0;
            float speed = 0.15f;
        };
    }
}

#endif