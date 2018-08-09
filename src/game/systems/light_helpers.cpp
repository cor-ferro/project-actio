#include "light_helpers.h"
#include "../world.h"

namespace game {
    namespace systems {
        LightHelpers::LightHelpers(World *world) : systems::BaseSystem(world) {}

        void LightHelpers::configure(entityx::EventManager &event_manager) {
            event_manager.subscribe<events::LightAdd>(*this);
            event_manager.subscribe<events::LightRemove>(*this);
            event_manager.subscribe<events::LightHelperShow>(*this);
        }

        void LightHelpers::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
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

                    events.emit<events::RenderSetupModel>(helper);
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

        void LightHelpers::receive(const events::LightAdd &event) {
            entityx::Entity lightEntity = event.entity;

            if (lightEntity.has_component<components::LightPoint>()) {
                newEntities.push(lightEntity);
            }
        }

        void LightHelpers::receive(const events::LightRemove &event) {
            entityx::Entity lightEntity = event.entity;

            if (lightEntity.has_component<components::LightPoint>()) {
                auto light = components::get<components::LightPoint>(lightEntity);
                auto lightHelper = components::get<components::LightHelper>(lightEntity);
            }
        }

        void LightHelpers::receive(const events::LightHelperShow &event) {
            isChangeStatus = true;
            isShowHelpers = event.value;
        }

        void LightHelpers::showHelpers(ex::EntityManager &es) {
            ex::ComponentHandle<components::LightHelper> helper;

            for (ex::Entity entity : es.entities_with_components(helper)) {
                entity.assign<components::Renderable>();
            }
        }

        void LightHelpers::hideHelpers(ex::EntityManager &es) {
            ex::ComponentHandle<components::LightHelper> helper;
            ex::ComponentHandle<components::Renderable> renderable;

            for (ex::Entity entity : es.entities_with_components(helper, renderable)) {
                entity.remove<components::Renderable>();
            }
        }

        void LightHelpers::updateHelperPositions(ex::EntityManager &es) {
            ex::ComponentHandle<components::LightHelper> helper;
            ex::ComponentHandle<components::Renderable> renderable;

            for (ex::Entity entity : es.entities_with_components(helper, renderable)) {
                auto lightTransform = components::get<components::Transform>(helper->entity);
                auto helperTransform = components::get<components::Transform>(helper->entity);

                helperTransform->setPosition(lightTransform->getPosition());
            }
        }
    }
}