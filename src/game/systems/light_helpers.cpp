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
//            while (!newDirectionalEntities.empty()) {
//                entityx::Entity lightEntity = newDirectionalEntities.top();
//
//                newDirectionalEntities.pop();
//
//                if (lightEntity.valid()) {
//                    setupDirectionalLight(lightEntity);
//                }
//            }

            while (!newPointEntities.empty()) {
                entityx::Entity lightEntity = newPointEntities.top();

                newPointEntities.pop();

                if (lightEntity.valid()) {
                    setupPointLight(lightEntity);
                }
            }

//            while (!newSpotEntities.empty()) {
//                entityx::Entity lightEntity = newSpotEntities.top();
//
//                newSpotEntities.pop();
//
//                if (lightEntity.valid()) {
//                    setupSpotLight(lightEntity);
//                }
//            }

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
            ex::ComponentHandle<components::Transform> helperTransform;
            ex::ComponentHandle<components::Renderable> renderable;

            for (ex::Entity entity : es.entities_with_components(helper, helperTransform, renderable)) {
                auto lightTransform = components::get<components::Transform>(helper->entity);
                helperTransform->setPosition(lightTransform->getPosition());
            }
        }

        void LightHelpers::setupDirectionalLight(ex::Entity& lightEntity) {

        }

        void LightHelpers::setupPointLight(ex::Entity& lightEntity) {
            auto light = components::get<components::LightPoint>(lightEntity);

            float radius = light->getRadius();
            const vec3 &position = light->getPosition();

            std::shared_ptr<Mesh> mesh = Mesh::Create();

            GeometryBuilder::SphereDescription sphereDesc;
            sphereDesc.radius = 1.0f;
            sphereDesc.widthSegments = 4;
            sphereDesc.heightSegments = 4;
            GeometryBuilder::Sphere(mesh->geometry, sphereDesc);
            mesh->material->setDiffuse(0.0f, 1.0f, 0.0f);
            mesh->material->setWireframe(true);

            game::WorldObject *helper = world->createObject();
            ex::Entity entity = helper->getEntity();

            helper->addComponent<components::LightHelper>(lightEntity);

            if (isShowHelpers) {
                world->showObject(helper);
            }

            lightEntity.assign<components::Helper>(entity);

            world->setObjectMesh(helper, mesh);
            world->spawn(helper, position);
            world->setupRenderMesh(entity);
        }

        void LightHelpers::setupSpotLight(ex::Entity& lightEntity) {
            auto light = components::get<components::LightSpot>(lightEntity);

//            float radius = light->getRadius();
             const vec3 &position = light->getPosition();
             const vec3 &direction = light->getDirection();

            std::shared_ptr<Mesh> mesh = Mesh::Create();

            GeometryBuilder::ConeDescription coneDesc;
            coneDesc.radius = 2.0f;
            coneDesc.height = 2.0f;
            coneDesc.radialSegments = 16;
            coneDesc.heightSegments = 16;
            GeometryBuilder::Cone(mesh->geometry, coneDesc);

            mesh->material->setDiffuse(0.0f, 1.0f, 0.0f);
            mesh->material->setWireframe(true);

            game::WorldObject *helper = world->createObject();
            ex::Entity entity = helper->getEntity();

            helper->addComponent<components::LightHelper>(lightEntity);

            if (isShowHelpers) {
                world->showObject(helper);
            }

            lightEntity.assign<components::Helper>(entity);

            world->setObjectMesh(helper, mesh);
            world->spawn(helper, position);
            world->setupRenderMesh(entity);
        }
    }
}