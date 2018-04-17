//
// Created by demitriy on 4/17/18.
//

#ifndef ACTIO_LIGHT_HELPERS_H
#define ACTIO_LIGHT_HELPERS_H

#include <entityx/entityx/System.h>
#include <stack>
#include "../events/light_add.h"
#include "../events/light_remove.h"
#include "../components/light_point.h"
#include "../../core/geometry_primitive.h"
#include "../../core/mesh.h"
#include "../components/model.h"
#include "../components/base.h"
#include "../events/render_setup_mesh.h"
#include "../components/transform.h"
#include "../components/light_helper.h"
#include "../components/renderable.h"
#include "../events/light_helper_show.h"
#include "../components/helper.h"

namespace game {
    namespace systems {
        using namespace entityx;

        class LightHelpers : public entityx::System<LightHelpers>, public entityx::Receiver<LightHelpers> {
        public:
            void configure(entityx::EventManager &event_manager) override {
                event_manager.subscribe<events::LightAdd>(*this);
                event_manager.subscribe<events::LightRemove>(*this);
                event_manager.subscribe<events::LightHelperShow>(*this);
            }

            void update(EntityManager &es, EventManager &events, TimeDelta dt) override {
                while (!newEntities.empty()) {
                    entityx::Entity lightEntity = newEntities.top();

                    newEntities.pop();

                    if (lightEntity.valid()) {
                        auto light = components::get<components::LightPoint>(lightEntity);

                        float radius = light->getRadius();
                        vec3 position = light->getPosition();

                        Mesh *mesh = Mesh::Create();
                        GeometryPrimitive::Sphere(mesh->geometry, radius, 16, 16, 0.0f, glm::two_pi<float>(), 0.0f, 3.14f);
                        mesh->material.setDiffuse(0.0f, 1.0f, 0.0f);
                        mesh->material.setWireframe(true);

                        entityx::Entity helper = es.create();

                        helper.assign<components::LightHelper>(lightEntity);
                        helper.assign<components::Model>(mesh);
                        helper.assign<components::Renderable>();
                        helper.assign<components::Transform>(position);

                        lightEntity.assign<components::Helper>(helper);

                        events.emit<events::RenderSetupMesh>(helper, mesh);
                    }
                }

                if (isChangeStatus) {
                    console::info("isChangeStatus");
                    if (showHelpers) {
                        ComponentHandle<components::LightHelper> helper;

                        for (Entity entity : es.entities_with_components(helper)) {
                            entity.assign<components::Renderable>();
                        }
                    } else {
                        ComponentHandle<components::LightHelper> helper;
                        ComponentHandle<components::Renderable> renderable;

                        for (Entity entity : es.entities_with_components(helper, renderable)) {
                            entity.remove<components::Renderable>();
                            console::info("remove renderable");
                        }
                    }

                    isChangeStatus = false;
                }
            }

            void receive(const events::LightAdd &event) {
                entityx::Entity lightEntity = event.entity;

                if (lightEntity.has_component<components::LightPoint>()) {
                    newEntities.push(lightEntity);
                }
            }

            void receive(const events::LightRemove &event) {
                entityx::Entity lightEntity = event.entity;

                if (lightEntity.has_component<components::LightPoint>()) {
                    auto light = components::get<components::LightPoint>(lightEntity);
                    auto lightHelper = components::get<components::LightHelper>(lightEntity);
                }
            }

            void receive(const events::LightHelperShow &event) {
                isChangeStatus = true;
                showHelpers = event.value;
            }

        private:
            std::stack<entityx::Entity> newEntities;
            bool isChangeStatus = false;
            bool showHelpers = false;
        };
    }
}

#endif //ACTIO_LIGHT_HELPERS_H
