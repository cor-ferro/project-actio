//
// Created by demitriy on 3/30/18.
//

#ifndef ACTIO_BALL_SHOOT_H
#define ACTIO_BALL_SHOOT_H

#include <entityx/entityx/System.h>
#include <stack>
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

namespace game {
    namespace systems {
        using namespace entityx;

        class BallShoot : public entityx::System<BallShoot>, public entityx::Receiver<BallShoot> {
        public:
            void configure(EventManager &event_manager) {
                event_manager.subscribe<events::KeyPress>(*this);
            }

            void update(EntityManager &es, EventManager &events, TimeDelta dt) override {
                vec3 cameraPosition;

                es.each<components::Camera>([&cameraPosition](entityx::Entity entity, components::Camera &camera) {
                    cameraPosition = camera.getPosition();
                });

                while (!newItems.empty()) {
                    console::info("newItems not empty");
                    newItems.pop();

                    entityx::Entity ball = es.create();

                    Material::Phong material;
                    material.setDiffuse(0.0f, 1.0f, 0.0f);
                    Geometry geometry = Geometry::Sphere(5.0f, 16, 16, 0.0f, glm::two_pi<float>(), 0.0f, 3.14f);
                    Mesh *mesh = Mesh::Create(geometry, material);

                    ball.assign<components::Model>(mesh);
                    ball.assign<components::Renderable>();
                    ball.assign<components::Transform>(vec3(0.0f));

                    events.emit<events::PhysicCreate>(ball);

                    console::warn("create sphere");
                }
            }

            void receive(const events::KeyPress &event) {
//                console::info("event: %i %i %i %i", event.key, event.scancode, event.action, event.mods);
                if (event.key == 32 && event.action == 1) {
                    newItems.push(1);
                }
            }
        private:
            std::stack<int> newItems;
        };
    }
}

#endif //ACTIO_BALL_SHOOT_H
