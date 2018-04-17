//
// Created by demitriy on 3/30/18.
//

#ifndef ACTIO_BALL_SHOOT_H
#define ACTIO_BALL_SHOOT_H

#include <entityx/entityx/System.h>
#include <stack>
#include <glm/glm/gtc/random.inl>
#include "../events/key_press.h"
#include "../../lib/console.h"
#include "../../lib/types.h"
#include "../components/physic.h"
#include "../components/transform.h"
#include "../components/camera.h"
#include "../events/physic_create.h"
#include "../../core/mesh.h"
#include "../components/model.h"
#include "../components/renderable.h"
#include "../events/render_setup_mesh.h"
#include "../events/mouse_press.h"
#include "../events/physic_force.h"
#include "../../core/geometry_primitive.h"

namespace game {
    namespace systems {
        using namespace entityx;

        class BallShoot : public entityx::System<BallShoot>, public entityx::Receiver<BallShoot> {
        public:
            void configure(EventManager &event_manager) {
                event_manager.subscribe<events::KeyPress>(*this);
                event_manager.subscribe<events::MousePress>(*this);
            }

            void update(EntityManager &es, EventManager &events, TimeDelta dt) override {
                vec3 cameraPosition, cameraTarget;

                es.each<components::Camera>([&cameraPosition, &cameraTarget](entityx::Entity entity, components::Camera &camera) {
                    cameraPosition = camera.getPosition();
                    cameraTarget = camera.getTarget();
                });

                while (!newItems.empty()) {
                    newItems.pop();

                    entityx::Entity ball = es.create();

                    float radius = 1.0f;
                    Mesh *mesh = Mesh::Create();

                    GeometryPrimitive::Sphere(mesh->geometry, radius, 16, 16, 0.0f, glm::two_pi<float>(), 0.0f, 3.14f);
                    mesh->material.setDiffuse(0.0f, 1.0f, 0.0f);

                    ball.assign<components::Model>(mesh);
                    ball.assign<components::Transform>(cameraPosition);

                    events.emit<events::PhysicCreateSphere>(ball, radius);
                    events.emit<events::PhysicForce>(ball, cameraTarget, 30.0f);
                    events.emit<events::RenderSetupMesh>(ball, mesh);
                }
            }

            void receive(const events::KeyPress &event) {
                if (event.key == InputHandler::KEY_E && event.action == 1) {
                    newItems.push(1);
                }
            }

            void receive(const events::MousePress &event) {
//                if (event.button == 0 && event.action == 1) {
//                    newItems.push(1);
//                }
            }
        private:
            std::stack<int> newItems;
        };
    }
}

#endif //ACTIO_BALL_SHOOT_H
