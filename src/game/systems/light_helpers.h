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
#include "../events/render_create_mesh.h"
#include "../components/transform.h"
#include "../components/light_helper.h"
#include "../components/renderable.h"
#include "../events/light_helper_show.h"
#include "../components/helper.h"
#include "../context.h"
#include "base.h"

namespace game {
    namespace systems {
        namespace ex = entityx;

        class LightHelpers
                : public systems::BaseSystem
                  , public entityx::System<LightHelpers>
                  , public entityx::Receiver<LightHelpers> {
        public:
            explicit LightHelpers(Context *context) : systems::BaseSystem(context) {}

            void configure(entityx::EventManager &event_manager) override {
                event_manager.subscribe<events::LightAdd>(*this);
                event_manager.subscribe<events::LightRemove>(*this);
                event_manager.subscribe<events::LightHelperShow>(*this);
            }

            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override {
                while (!newEntities.empty()) {
                    entityx::Entity lightEntity = newEntities.top();

                    newEntities.pop();

                    if (lightEntity.valid()) {
                        auto light = components::get<components::LightPoint>(lightEntity);

                        float radius = light->getRadius();
                        vec3 position = light->getPosition();

                        Mesh *mesh = Mesh::Create();
                        GeometryPrimitive::Sphere(mesh->geometry, radius, 16, 16, 0.0f, glm::two_pi<float>(), 0.0f,
                                                  3.14f);
                        mesh->material->setDiffuse(0.0f, 1.0f, 0.0f);
                        mesh->material->setWireframe(true);

                        entityx::Entity helper = es.create();

                        helper.assign<components::LightHelper>(lightEntity);
                        helper.assign<components::Model>(mesh);
                        helper.assign<components::Transform>(position);

                        if (isShowHelpers) {
                            helper.assign<components::Renderable>();
                        }

                        lightEntity.assign<components::Helper>(helper);

                        events.emit<events::RenderCreateMesh>(helper, mesh);
                    }
                }

                if (isChangeStatus) {
                    if (isShowHelpers) {
                        showHelpers(es);
                    } else {
                        hideHelpers(es);
                    }

                    isChangeStatus = false;
                }

                if (isShowHelpers) {
                    updateHelperPositions(es);
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
                isShowHelpers = event.value;
            }

        private:
            void showHelpers(ex::EntityManager &es) {
                ex::ComponentHandle<components::LightHelper> helper;

                for (ex::Entity entity : es.entities_with_components(helper)) {
                    entity.assign<components::Renderable>();
                }
            }

            void hideHelpers(ex::EntityManager &es) {
                ex::ComponentHandle<components::LightHelper> helper;
                ex::ComponentHandle<components::Renderable> renderable;

                for (ex::Entity entity : es.entities_with_components(helper, renderable)) {
                    entity.remove<components::Renderable>();
                }
            }

            void updateHelperPositions(ex::EntityManager &es) {
                ex::ComponentHandle<components::LightHelper> helper;
                ex::ComponentHandle<components::Renderable> renderable;

                for (ex::Entity entity : es.entities_with_components(helper, renderable)) {
                    auto lightTransform = components::get<components::Transform>(helper->entity);
                    auto helperTransform = components::get<components::Transform>(helper->entity);

                    helperTransform->setPosition(lightTransform->getPosition());
                }
            }

            std::stack<entityx::Entity> newEntities;
            bool isChangeStatus = false;
            bool isShowHelpers = false;
        };
    }
}

#endif //ACTIO_LIGHT_HELPERS_H
