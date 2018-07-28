//
// Created by demitriy on 7/8/18.
//

#ifndef ACTIO_GAME_SYSTEMS_SKY_H
#define ACTIO_GAME_SYSTEMS_SKY_H

#include <entityx/entityx/System.h>
#include "base.h"
#include "../../lib/console.h"

namespace game {
    namespace systems {
        namespace ex = entityx;

        class Sky
                : public systems::BaseSystem
                  , public entityx::System<Sky>
                  , public entityx::Receiver<Sky> {
        public:
            explicit Sky(Context *context) : systems::BaseSystem(context) {}

            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override {
                if (!isFirstCall) {
                    ex::Entity sun = es.create();

                    float radius = 1.0f;

                    Mesh *mesh = Mesh::Create();
                    GeometryPrimitive::Circle(mesh->geometry, radius, 16, .0f, glm::two_pi<float>());
                    mesh->material->setDiffuse(0.0f, 1.0f, 0.0f);

                    sun.assign<components::Model>(mesh);
                    sun.assign<components::Transform>(vec3(0.0f));

                    events.emit<events::RenderCreateMesh>(sun, mesh);

                    isFirstCall = true;
                }
            }

        private:
            bool isFirstCall = false;
        };
    }
}

#endif //ACTIO_GAME_SYSTEMS_SKY_H
