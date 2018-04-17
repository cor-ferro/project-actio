#ifndef GAME_SYSTEMS_CAMERA_H_
#define GAME_SYSTEMS_CAMERA_H_

#include <entityx/entityx.h>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include "../events/key_press.h"
#include "../components/transform.h"
#include "../components/camera.h"
#include "../components/target.h"
#include "../../lib/console.h"
#include "../../lib/input_handler.h"
#include "../events/mouse_press.h"
#include "../components/model.h"
#include "../components/controlled.h"

namespace game {
    namespace systems {
        using namespace entityx;

        class Camera : public entityx::System<Camera>, public entityx::Receiver<Camera> {
        public:
            explicit Camera(InputHandler *ih) : inputHandler(ih) {}

            void configure(EntityManager &es, entityx::EventManager &event_manager) {
                event_manager.subscribe<events::KeyPress>(*this);
                event_manager.subscribe<events::MousePress>(*this);

                entityManager = &es;
            }

            void update(EntityManager &es, EventManager &events, TimeDelta dt) override {
                vec3 characterPosition(0.0f);

                ComponentHandle<components::Model> model;
                ComponentHandle<components::Controlled> control;
                ComponentHandle<components::Transform> characterTransform;
                for (Entity entity : es.entities_with_components(model, control, characterTransform)) {
                    characterPosition = characterTransform->getPosition();
                }

                ComponentHandle<components::Camera> camera;
                ComponentHandle<components::Transform> cameraTransform;
                ComponentHandle<components::Target> cameraTarget;
                for (Entity entity : es.entities_with_components(camera, cameraTransform, cameraTarget)) {
                    updateCameraSide(camera, characterPosition, dt);

                    camera->position_ = newPosition;
                    camera->view_ = glm::lookAt(newPosition, newTarget, vec3(0.0f, 1.0f, 0.0f));
                }
            }

            void updateCamera1(ComponentHandle<components::Camera> camera) {
                float speedX = glm::abs(inputHandler->mouseMoved.x * inputHandler->sensetivity.speedFactor);
                float speedY = glm::abs(inputHandler->mouseMoved.y * inputHandler->sensetivity.speedFactor);

                float angleX = inputHandler->mouseMoved.y * 0.03f * -glm::pow(speedX, inputHandler->sensetivity.sensetivity);
                float angleY = inputHandler->mouseMoved.x * 0.03f * -glm::pow(speedY, inputHandler->sensetivity.sensetivity);

                if (angleY > 100.0f) {
                    console::info("over angle y %f %i", angleY, inputHandler->mouseMoved.x);
                    console::info("details %i %i %i", inputHandler->mouseMoved.x, inputHandler->mouseStart.x, inputHandler->mouse.x);
                    angleY = 10.0f;
                }

                if (angleX > 100.0f) {
                    console::info("over angle x %i", angleX);
                    console::info("details %i %i %i", inputHandler->mouseMoved.y, inputHandler->mouseStart.y, inputHandler->mouse.y);
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

                if (inputHandler->isPress(InputHandler::KEY_W)) cameraPosition -= speed * front;
                if (inputHandler->isPress(InputHandler::KEY_S)) cameraPosition += speed * front;
                if (inputHandler->isPress(InputHandler::KEY_A)) cameraPosition += left * speed;
                if (inputHandler->isPress(InputHandler::KEY_D)) cameraPosition -= left * speed;
                if (inputHandler->isPress(InputHandler::KEY_C)) cameraPosition -= vec3(0.0f, 0.1f, 0.0f);
                if (inputHandler->isPress(InputHandler::KEY_SPACE)) cameraPosition += vec3(0.0f, 0.1f, 0.0f);

                camera->setRotation(cameraRotation);
                camera->setPosition(cameraPosition);
//                setIsChanged(true);
            }

            void updateCameraFree(ComponentHandle<components::Camera> camera, TimeDelta dt) {
                vec3 cameraPosition = camera->getPosition();
                const vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

                if (isCameraRotateEnabled) {
                    yaw += inputHandler->mouseMoved.x * -0.03f;
                    pitch += inputHandler->mouseMoved.y * 0.03f;

                    if (pitch > 89.0f)
                        pitch = 89.0f;
                    if (pitch < -89.0f)
                        pitch = -89.0f;

                    camera->target_.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                    camera->target_.y = sin(glm::radians(pitch));
                    camera->target_.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
                    camera->target_ = glm::normalize(camera->target_);
                }

                if (isCameraTranslationEnabled) {
                    vec3 forwardDirection = camera->target_ * speed;
                    vec3 sideDirection = glm::normalize(glm::cross(camera->target_, cameraUp)) * speed;

                    if (inputHandler->isPress(InputHandler::KEY_W)) cameraPosition += forwardDirection;
                    if (inputHandler->isPress(InputHandler::KEY_S)) cameraPosition -= forwardDirection;
                    if (inputHandler->isPress(InputHandler::KEY_A)) cameraPosition -= sideDirection;
                    if (inputHandler->isPress(InputHandler::KEY_D)) cameraPosition += sideDirection;
                    if (inputHandler->isPress(InputHandler::KEY_C)) cameraPosition -= vec3(0.0f, 0.1f, 0.0f);
                    if (inputHandler->isPress(InputHandler::KEY_SPACE)) cameraPosition += vec3(0.0f, 0.1f, 0.0f);
                }

                newPosition = cameraPosition;
                newTarget = cameraPosition + camera->target_;
                setIsChanged(true);
            }

            void updateCameraSide(ComponentHandle<components::Camera> camera, vec3 &sideTarget, TimeDelta dt) {
                vec3 cameraPosition = camera->getPosition();
                const vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

                if (isCameraTranslationEnabled) {
                    vec3 forwardDirection = camera->target_ * speed;
                    vec3 sideDirection = glm::normalize(glm::cross(camera->target_, cameraUp)) * speed;

                    if (inputHandler->isPress(InputHandler::KEY_W)) cameraPosition += forwardDirection;
                    if (inputHandler->isPress(InputHandler::KEY_S)) cameraPosition -= forwardDirection;
                    if (inputHandler->isPress(InputHandler::KEY_A)) cameraPosition -= sideDirection;
                    if (inputHandler->isPress(InputHandler::KEY_D)) cameraPosition += sideDirection;
                    if (inputHandler->isPress(InputHandler::KEY_C)) cameraPosition -= vec3(0.0f, 0.1f, 0.0f);
                    if (inputHandler->isPress(InputHandler::KEY_SPACE)) cameraPosition += vec3(0.0f, 0.1f, 0.0f);
                }

                newPosition = vec3(-20.0f, 4.0f, sideTarget.z + 10.0f);
                newTarget = vec3(sideTarget.x, 4.0f, sideTarget.z + 10.0f);
                setIsChanged(true);
            }

            void lookAt(vec3 target) {
//                ComponentHandle<components::Camera> camera;
//
//                for (Entity entity : entityManager->entities_with_components(camera)) {
//                    newTarget = target;
//                    camera->view_ = glm::lookAt(camera->getPosition(), target, vec3(0.0f, 1.0f, 0.0f));
//
//                    vec3 scale;
//                    quat orientation;
//                    vec3 translation;
//                    vec3 skew;
//                    vec4 perspective;
//
//                    glm::decompose(camera->view_, scale, orientation, translation, skew, perspective);
//                    glm::vec3 euler = glm::eulerAngles(orientation);
//
//                    yaw = -90.0f - glm::degrees(euler.x);
//                    pitch = glm::degrees(euler.y);
//
//                }
            }

            void setSettings(float fov, float aspect, float near, float far) {
                ComponentHandle<components::Camera> camera;

                for (Entity entity : entityManager->entities_with_components(camera)) {
                    camera->setParam(CameraParam::FOV, fov);
                    camera->setParam(CameraParam::ASPECT, aspect);
                    camera->setParam(CameraParam::NEAR, near);
                    camera->setParam(CameraParam::FAR, far);
                    camera->updateProjection();
                }
            }

            void receive(const events::KeyPress &event) {

            }

            void receive(const events::MousePress &event) {
                if (event.button == InputHandler::MOUSE_BUTTON_LEFT) {
                    if (event.action == InputHandler::KEY_PRESS) {
                        isCameraRotateEnabled = true;
                        isCameraTranslationEnabled = true;
                    } else if (event.action == InputHandler::KEY_RELEASE) {
                        isCameraRotateEnabled = false;
                        isCameraTranslationEnabled = false;
                    }
                }
            }

            void setIsChanged(bool value) {
                isChanged = value;
            }

        private:
            EntityManager *entityManager = nullptr;
            InputHandler *inputHandler;

            vec3 newPosition;
            vec3 newTarget;

            float yaw = 0.0f;
            float pitch = 0.0f;

            float speed = 0.15f;

            bool isCameraRotateEnabled = false;
            bool isCameraTranslationEnabled = false;

            bool isChanged = false;
        };
    }
}

#endif