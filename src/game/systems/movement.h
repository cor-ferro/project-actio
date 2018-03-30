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

namespace game {
    namespace systems {
        using namespace entityx;

        class Movement : public entityx::System<Movement> {
        public:
            explicit Movement(InputHandler * ih) : ih_(ih) {

            }

            void configure(entityx::EventManager &event_manager) {
                ih_->subscribeKeyPress([&event_manager](int key, int scancode, int action, int mods) {
                    event_manager.emit<events::KeyPress>(key, scancode, action, mods);
                });
            }

            void update(EntityManager &es, EventManager &events, TimeDelta dt) override {
                ComponentHandle<components::Camera> camera;
                ComponentHandle<components::Transform> cameraTransform;
                ComponentHandle<components::Target> cameraTarget;

                for (Entity entity : es.entities_with_components(camera, cameraTransform, cameraTarget)) {
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

                    cameraRotation.x+= angleX;
                    cameraRotation.y+= angleY;

                    float radRotationX = glm::radians(cameraRotation.x);
                    float radRotationY = glm::radians(cameraRotation.y);

                    front.x = -glm::cos(radRotationX) * glm::sin(radRotationY);
                    front.y = glm::sin(radRotationX);
                    front.z = glm::cos(radRotationX) * glm::cos(radRotationY);
                    front = glm::normalize(front);

                    vec3 left = glm::normalize(glm::cross(front, cameraUp));

                    if (ih_->isPress(InputHandler::KEY_W)) cameraPosition-= speed * front;
                    if (ih_->isPress(InputHandler::KEY_S)) cameraPosition+= speed * front;
                    if (ih_->isPress(InputHandler::KEY_A)) cameraPosition+= left * speed;
                    if (ih_->isPress(InputHandler::KEY_D)) cameraPosition-= left * speed;
                    if (ih_->isPress(InputHandler::KEY_C)) cameraPosition-= vec3(0.0f, 0.1f, 0.0f);
                    if (ih_->isPress(InputHandler::KEY_SPACE)) cameraPosition+= vec3(0.0f, 0.1f, 0.0f);

                    camera->setRotation(cameraRotation);
                    camera->setPosition(cameraPosition);
                }
            }

        private:
            InputHandler * ih_;

            float speed = 0.5f;
        };
    }
}

#endif