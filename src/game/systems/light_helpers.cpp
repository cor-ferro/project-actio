#include "light_helpers.h"
#include "../components/mesh.h"
#include "../events/object_create.h"

namespace game {
    namespace systems {
        LightHelpers::LightHelpers(Context& context) : systems::BaseSystem(context) {}

        void LightHelpers::configure(entityx::EventManager& event_manager) {
            event_manager.subscribe<events::LightAdd>(*this);
            event_manager.subscribe<events::LightRemove>(*this);
            event_manager.subscribe<events::LightHelperShow>(*this);
        }

        void LightHelpers::update(ex::EntityManager& es, ex::EventManager& events, ex::TimeDelta dt) {
            processQueue<entityx::Entity>(newDirectionalEntities, [this, &es, &events](entityx::Entity& lightEntity) {

            });

            processQueue<entityx::Entity>(newPointEntities, [this, &es, &events](entityx::Entity& lightEntity) {
                auto light = components::get<components::LightPoint>(lightEntity);

                float radius = light->getRadius();
                const vec3& position = light->getPosition();

                MeshHandle mesh = m_context.meshes().create();

                GeometryBuilder::SphereDescription sphereDesc;
                sphereDesc.radius = 1.0f;
                sphereDesc.widthSegments = 4;
                sphereDesc.heightSegments = 4;
                GeometryBuilder::Sphere(mesh->geometry, sphereDesc);
                mesh->material->setDiffuse(0.0f, 1.0f, 0.0f);
                mesh->material->setWireframe(true);

                auto entity = m_context.createObject(es);

                entity.assign<components::LightHelper>(lightEntity);

                if (isShowHelpers) {
                    entity.assign<components::Renderable>();
                }

                lightEntity.assign<components::Helper>(entity);

                auto physicActor = m_context.physic().createStaticActor(position);

                entity.assign<components::Mesh>(mesh);
                entity.assign<components::PhysicActor>(physicActor, entity);

                events.emit<events::ObjectCreate>(entity);
            });

            processQueue<entityx::Entity>(newSpotEntities, [this, &es, &events](entityx::Entity& lightEntity) {
                if (lightEntity.valid()) {
                    auto light = components::get<components::LightSpot>(lightEntity);

                    const vec3& position = light->getPosition();
                    const vec3& direction = light->getDirection();

                    MeshHandle mesh = m_context.meshes().create();

                    GeometryBuilder::ConeDescription coneDesc;
                    coneDesc.radius = 2.0f;
                    coneDesc.height = 2.0f;
                    coneDesc.radialSegments = 16;
                    coneDesc.heightSegments = 16;
                    GeometryBuilder::Cone(mesh->geometry, coneDesc);

                    mesh->material->setDiffuse(0.0f, 1.0f, 0.0f);
                    mesh->material->setWireframe(true);

                    auto entity = m_context.createObject(es);

                    entity.assign<components::LightHelper>(lightEntity);

                    if (isShowHelpers) {
                        entity.assign<components::Renderable>();
                    }

                    lightEntity.assign<components::Helper>(entity);

                    auto physicActor = m_context.physic().createStaticActor(position);

                    entity.assign<components::Mesh>(mesh);
                    entity.assign<components::PhysicActor>(physicActor, entity);

                    events.emit<events::ObjectCreate>(entity);
                }
            });

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

        void LightHelpers::receive(const events::LightAdd& event) {
            entityx::Entity lightEntity = event.entity;

            bool a = lightEntity.has_component<components::LightDirectional>();
            bool b = lightEntity.has_component<components::LightPoint>();
            bool c = lightEntity.has_component<components::LightSpot>();

//            if (lightEntity.has_component<components::LightDirectional>()) {
//                newDirectionalEntities.push(lightEntity);
//            }
//
            if (lightEntity.has_component<components::LightPoint>()) {
                newPointEntities.push(lightEntity);
            }
//
//            if (lightEntity.has_component<components::LightSpot>()) {
//                newSpotEntities.push(lightEntity);
//            }
        }

        void LightHelpers::receive(const events::LightRemove& event) {
            entityx::Entity lightEntity = event.entity;

            if (lightEntity.has_component<components::LightPoint>()) {
                auto light = components::get<components::LightPoint>(lightEntity);
                auto lightHelper = components::get<components::LightHelper>(lightEntity);
            }
        }

        void LightHelpers::receive(const events::LightHelperShow& event) {
            show(event.value);
        }

        void LightHelpers::showHelpers(ex::EntityManager& es) {
            ex::ComponentHandle<components::LightHelper> helper;

            for (ex::Entity entity : es.entities_with_components(helper)) {
                entity.assign<components::Renderable>();
            }
        }

        void LightHelpers::hideHelpers(ex::EntityManager& es) {
            ex::ComponentHandle<components::LightHelper> helper;
            ex::ComponentHandle<components::Renderable> renderable;

            for (ex::Entity entity : es.entities_with_components(helper, renderable)) {
                entity.remove<components::Renderable>();
            }
        }

        void LightHelpers::updateHelperPositions(ex::EntityManager& es) {
            es.each<components::LightHelper, components::PhysicActor, components::Renderable>([](
                    ex::Entity entity,
                    components::LightHelper& helper,
                    components::PhysicActor& physicActor,
                    components::Renderable&
            ) {
                auto lightPhysicActor = components::get<components::PhysicActor>(helper.entity);

                physicActor.setPosition(lightPhysicActor->getPosition());
            });
        }

        void LightHelpers::show(bool value) {
            isChangeStatus = true;
            isShowHelpers = value;
        }
    }
}