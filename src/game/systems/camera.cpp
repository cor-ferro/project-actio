#include "camera.h"

namespace game {
    namespace systems {
        Camera::Camera(Context& context) : systems::BaseSystem(context) {}

        void Camera::configure(ex::EntityManager& es, entityx::EventManager& events) {
            events.subscribe<events::KeyPress>(*this);
            events.subscribe<events::MousePress>(*this);

            entityManager = &es;
        }

        void Camera::start(ex::EntityManager& es, ex::EventManager& events, ex::TimeDelta dt) {
            newPosition = vec3(0.0f, 75.0f, 35.0f); // z-hack fix
            newTarget = vec3(0.0f, 0.0f, 0.0f);

            auto &pos = this->newPosition;

            m_context.input().onMouseScroll.connect([&pos](double offsetx, double offsety) {
                pos.y += static_cast<float>(offsety) * 3.0f;
            });
        }

        void Camera::update(ex::EntityManager& es, ex::EventManager& events, ex::TimeDelta dt) {
            vec3 characterPosition(0.0f);

            ex::ComponentHandle<components::UserControl> userControl;
            ex::ComponentHandle<components::Transform> characterTransform;
            for (ex::Entity entity : es.entities_with_components(userControl, characterTransform)) {
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

                const InputManager& input = m_context.input();

                const vec3 original(input.mouse.x, m_context.data.windowHeight - input.mouse.y, 0.0f);
                const vec4 viewport(0.0f, 0.0f, m_context.data.windowWidth, m_context.data.windowHeight);
                const mat4 frustum = glm::perspective(
                        glm::radians(camera->getParam(CameraParam::FOV)),
                        camera->getParam(CameraParam::ASPECT),
                        glm::abs(camera->position_.x),
                        glm::abs(camera->position_.x) + 1.0f
                );

                m_context.data.mouseWorldPosition = glm::unProject(original, camera->getView(), frustum, viewport);
            }
        }

        void Camera::updateCamera1(ex::ComponentHandle<components::Camera> camera) {
            InputManager& input = m_context.input();

            float speedX = glm::abs(input.mouseMoved.x * input.sensetivity.speedFactor);
            float speedY = glm::abs(input.mouseMoved.y * input.sensetivity.speedFactor);

            float angleX = input.mouseMoved.y * 0.03f * -glm::pow(speedX, input.sensetivity.sensetivity);
            float angleY = input.mouseMoved.x * 0.03f * -glm::pow(speedY, input.sensetivity.sensetivity);

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

            if (m_context.input().isPress(InputManager::KEY_W)) cameraPosition -= speed * front;
            if (m_context.input().isPress(InputManager::KEY_S)) cameraPosition += speed * front;
            if (m_context.input().isPress(InputManager::KEY_A)) cameraPosition += left * speed;
            if (m_context.input().isPress(InputManager::KEY_D)) cameraPosition -= left * speed;
            if (m_context.input().isPress(InputManager::KEY_C)) cameraPosition -= vec3(0.0f, 0.1f, 0.0f);
            if (m_context.input().isPress(InputManager::KEY_SPACE)) cameraPosition += vec3(0.0f, 0.1f, 0.0f);

            camera->setRotation(cameraRotation);
            camera->setPosition(cameraPosition);
        }

        void Camera::updateCameraFree(ex::ComponentHandle<components::Camera> camera, ex::TimeDelta dt) {
            InputManager& input = m_context.input();

            vec3 cameraPosition = camera->getPosition();
            const vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

            if (isCameraRotateEnabled) {
                yaw += input.mouseMoved.x * -0.03f;
                pitch += input.mouseMoved.y * 0.03f;

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

                if (m_context.input().isPress(InputManager::KEY_W)) cameraPosition += forwardDirection;
                if (m_context.input().isPress(InputManager::KEY_S)) cameraPosition -= forwardDirection;
                if (m_context.input().isPress(InputManager::KEY_A)) cameraPosition -= sideDirection;
                if (m_context.input().isPress(InputManager::KEY_D)) cameraPosition += sideDirection;
                if (m_context.input().isPress(InputManager::KEY_C)) cameraPosition -= vec3(0.0f, 0.1f, 0.0f);
                if (m_context.input().isPress(InputManager::KEY_SPACE)) cameraPosition += vec3(0.0f, 0.1f, 0.0f);
            }

            newPosition = cameraPosition;
            newTarget = cameraPosition + camera->target_;
        }

        void
        Camera::updateCameraSide(ex::ComponentHandle<components::Camera> camera, vec3& sideTarget, ex::TimeDelta dt) {
            vec3 cameraPosition = camera->getPosition();
            const vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

            if (isCameraTranslationEnabled) {
                vec3 forwardDirection = camera->target_ * speed;
                vec3 sideDirection = glm::normalize(glm::cross(camera->target_, cameraUp)) * speed;

                if (m_context.input().isPress(InputManager::KEY_W)) cameraPosition += forwardDirection;
                if (m_context.input().isPress(InputManager::KEY_S)) cameraPosition -= forwardDirection;
                if (m_context.input().isPress(InputManager::KEY_A)) cameraPosition -= sideDirection;
                if (m_context.input().isPress(InputManager::KEY_D)) cameraPosition += sideDirection;
                if (m_context.input().isPress(InputManager::KEY_C)) cameraPosition -= vec3(0.0f, 0.1f, 0.0f);
                if (m_context.input().isPress(InputManager::KEY_SPACE)) cameraPosition += vec3(0.0f, 0.1f, 0.0f);
            }

            vec3 originalPos = camera->getPosition();

            if (m_context.input().isPress(InputManager::MOUSE_BUTTON_MIDDLE)) {
                originalPos.x += m_context.input().mouseMoved.x * 0.2f;
                originalPos.z += m_context.input().mouseMoved.y;

                newPosition = vec3(originalPos.x, originalPos.y, originalPos.z);
                newTarget = vec3(originalPos.x, 0.0f, originalPos.z - 35.0f);
            }

            auto sp = m_context.input().mouseScroll;
        }

        void Camera::lookAt(vec3 target) {
            newTarget = target;
        }

        void Camera::setSettings(float fov, float aspect, float near, float far) {
            ex::ComponentHandle<components::Camera> camera;

            for (ex::Entity entity : entityManager->entities_with_components(camera)) {
                camera->setParam(CameraParam::FOV, fov);
                camera->setParam(CameraParam::ASPECT, aspect);
                camera->setParam(CameraParam::NEAR, near);
                camera->setParam(CameraParam::FAR, far);
                camera->updateProjection();
            }
        }

        void Camera::receive(const events::KeyPress& event) {

        }

        void Camera::receive(const events::MousePress& event) {

        }
    }
}