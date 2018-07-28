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
#include "../../core/geometry_primitive.h"
#include "../components/renderable.h"
#include "../components/base.h"
#include "../context.h"
#include "base.h"
#include "../components/user_control.h"

namespace game {
    namespace systems {
        namespace ex = entityx;

        class Camera
                : public systems::BaseSystem
                  , public entityx::System<Camera>
                  , public entityx::Receiver<Camera> {
        public:
            explicit Camera(Context *context) : systems::BaseSystem(context) {}

            void configure(ex::EntityManager &es, entityx::EventManager &events) {
                events.subscribe<events::KeyPress>(*this);
                events.subscribe<events::MousePress>(*this);

                entityManager = &es;
            }

            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override {
                InputHandler *input = worldContext->input;
                if (input == nullptr) return;

                vec3 characterPosition(0.0f);

                ex::ComponentHandle<components::Model> model;
                ex::ComponentHandle<components::UserControl> userControl;
                ex::ComponentHandle<components::Transform> characterTransform;
                for (ex::Entity entity : es.entities_with_components(model, userControl, characterTransform)) {
                    characterPosition = characterTransform->getPosition();
                }

                ex::ComponentHandle<components::Camera> camera;
                ex::ComponentHandle<components::Transform> cameraTransform;
                ex::ComponentHandle<components::Target> cameraTarget;

                ex::ComponentHandle<components::Transform> helperTransform;

                for (ex::Entity entity : es.entities_with_components(camera, cameraTransform, cameraTarget)) {
                    updateCameraSide(camera, characterPosition, dt);

                    cameraTransform->setPosition(newPosition);
                    cameraTarget->target = newTarget;

                    camera->position_ = newPosition;
                    camera->view_ = glm::lookAt(newPosition, newTarget, vec3(0.0f, 1.0f, 0.0f));

                    vec3 original(worldContext->mousePosition.x, worldContext->windowHeight - input->mouse.y, 0.0f);
                    vec4 viewport(0.0f, 0.0f, worldContext->windowWidth, worldContext->windowHeight);
                    mat4 frustum = glm::perspective(
                            glm::radians(camera->getParam(CameraParam::FOV)),
                            camera->getParam(CameraParam::ASPECT),
                            glm::abs(camera->position_.x),
                            glm::abs(camera->position_.x) + 1.0f
                    );

                    worldContext->mouseWorldPosition = glm::unProject(original, camera->getView(), frustum, viewport);
                }
            }

            void updateCamera1(ex::ComponentHandle<components::Camera> camera) {
                InputHandler *input = worldContext->input;
                if (input == nullptr) return;

                float speedX = glm::abs(input->mouseMoved.x * input->sensetivity.speedFactor);
                float speedY = glm::abs(input->mouseMoved.y * input->sensetivity.speedFactor);

                float angleX =
                        input->mouseMoved.y * 0.03f * -glm::pow(speedX, input->sensetivity.sensetivity);
                float angleY =
                        input->mouseMoved.x * 0.03f * -glm::pow(speedY, input->sensetivity.sensetivity);

                if (angleY > 100.0f) {
                    angleY = 10.0f;
                }

                if (angleX > 100.0f) {
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

                if (input->isPress(InputHandler::KEY_W)) cameraPosition -= speed * front;
                if (input->isPress(InputHandler::KEY_S)) cameraPosition += speed * front;
                if (input->isPress(InputHandler::KEY_A)) cameraPosition += left * speed;
                if (input->isPress(InputHandler::KEY_D)) cameraPosition -= left * speed;
                if (input->isPress(InputHandler::KEY_C)) cameraPosition -= vec3(0.0f, 0.1f, 0.0f);
                if (input->isPress(InputHandler::KEY_SPACE)) cameraPosition += vec3(0.0f, 0.1f, 0.0f);

                camera->setRotation(cameraRotation);
                camera->setPosition(cameraPosition);
            }

            void updateCameraFree(ex::ComponentHandle<components::Camera> camera, ex::TimeDelta dt) {
                InputHandler *input = worldContext->input;
                if (input == nullptr) return;

                vec3 cameraPosition = camera->getPosition();
                const vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

                if (isCameraRotateEnabled) {
                    yaw += input->mouseMoved.x * -0.03f;
                    pitch += input->mouseMoved.y * 0.03f;

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

                    if (input->isPress(InputHandler::KEY_W)) cameraPosition += forwardDirection;
                    if (input->isPress(InputHandler::KEY_S)) cameraPosition -= forwardDirection;
                    if (input->isPress(InputHandler::KEY_A)) cameraPosition -= sideDirection;
                    if (input->isPress(InputHandler::KEY_D)) cameraPosition += sideDirection;
                    if (input->isPress(InputHandler::KEY_C)) cameraPosition -= vec3(0.0f, 0.1f, 0.0f);
                    if (input->isPress(InputHandler::KEY_SPACE)) cameraPosition += vec3(0.0f, 0.1f, 0.0f);
                }

                newPosition = cameraPosition;
                newTarget = cameraPosition + camera->target_;
            }

            void updateCameraSide(ex::ComponentHandle<components::Camera> camera, vec3 &sideTarget, ex::TimeDelta dt) {
                InputHandler *input = worldContext->input;
                if (input == nullptr) return;

                vec3 cameraPosition = camera->getPosition();
                const vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

                if (isCameraTranslationEnabled) {
                    vec3 forwardDirection = camera->target_ * speed;
                    vec3 sideDirection = glm::normalize(glm::cross(camera->target_, cameraUp)) * speed;

                    if (input->isPress(InputHandler::KEY_W)) cameraPosition += forwardDirection;
                    if (input->isPress(InputHandler::KEY_S)) cameraPosition -= forwardDirection;
                    if (input->isPress(InputHandler::KEY_A)) cameraPosition -= sideDirection;
                    if (input->isPress(InputHandler::KEY_D)) cameraPosition += sideDirection;
                    if (input->isPress(InputHandler::KEY_C)) cameraPosition -= vec3(0.0f, 0.1f, 0.0f);
                    if (input->isPress(InputHandler::KEY_SPACE)) cameraPosition += vec3(0.0f, 0.1f, 0.0f);
                }

                newPosition = vec3(-20.0f, 4.0f, sideTarget.z);
                newTarget = vec3(sideTarget.x, 4.0f, sideTarget.z);
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
                ex::ComponentHandle<components::Camera> camera;

                for (ex::Entity entity : entityManager->entities_with_components(camera)) {
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

        private:
            ex::EntityManager *entityManager = nullptr;

            vec3 newPosition;
            vec3 newTarget;

            float yaw = 0.0f;
            float pitch = 0.0f;

            float speed = 0.15f;

            bool isCameraRotateEnabled = false;
            bool isCameraTranslationEnabled = false;
        };
    }
}

#endif