//
// Created by demitriy on 7/8/18.
//

#ifndef ACTIO_GAME_SYSTEMS_SKY_H
#define ACTIO_GAME_SYSTEMS_SKY_H

#include <entityx/entityx/System.h>
#include "../components/model.h"
#include "../components/transform.h"
#include "../events/render_setup_model.h"
#include "../../core/mesh.h"
#include "../../core/geometry_primitive.h"
#include "../../lib/console.h"
#include "base.h"

namespace game {
    namespace systems {
        namespace ex = entityx;
        namespace c = components;

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

                    sun.assign<c::Model>(mesh);
                    sun.assign<c::Transform>(vec3(0.0f));

                    events.emit<events::RenderSetupModel>(sun);

                    isFirstCall = true;
                }
            }

        private:
            bool isFirstCall = false;
        };
    }
}

#endif //ACTIO_GAME_SYSTEMS_SKY_H
