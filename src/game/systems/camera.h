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
#include "../../lib/input_manager.h"
#include "../events/mouse_press.h"
#include "../components/model.h"
#include "../components/controlled.h"
#include "../../core/geometry_builder.h"
#include "../components/renderable.h"
#include "../components/base.h"
#include "../context.h"
#include "base.h"
#include "../components/user_control.h"

namespace game {
    class World;

    namespace systems {
        namespace ex = entityx;

        class Camera
                : public systems::BaseSystem
                  , public entityx::System<Camera>
                  , public entityx::Receiver<Camera> {
        public:
            explicit Camera(Context& context);

            void configure(ex::EntityManager &es, entityx::EventManager &events) override;

            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;

            void updateCamera1(ex::ComponentHandle<components::Camera> camera);

            void updateCameraFree(ex::ComponentHandle<components::Camera> camera, ex::TimeDelta dt);

            void updateCameraSide(ex::ComponentHandle<components::Camera> camera, vec3 &sideTarget, ex::TimeDelta dt);

            void lookAt(vec3 target);

            void setSettings(float fov, float aspect, float near, float far);

            void receive(const events::KeyPress &event);

            void receive(const events::MousePress &event);

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