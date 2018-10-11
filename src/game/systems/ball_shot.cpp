#include "ball_shot.h"
#include "../world.h"

namespace game {
    namespace systems {
        BallShot::BallShot(World *world) : systems::BaseSystem(world) {}

        void BallShot::configure(ex::EventManager &event_manager) {
            event_manager.subscribe<events::KeyPress>(*this);
            event_manager.subscribe<events::MousePress>(*this);
        }

        void BallShot::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
            vec3 cameraPosition, cameraTarget;
            vec3 charPosition, charTarget;

            es.each<components::Camera>(
                    [&cameraPosition, &cameraTarget](ex::Entity entity, components::Camera &camera) {
                        cameraPosition = camera.getPosition();
                        cameraTarget = camera.getTarget();
                    });

            es.each<c::UserControl, components::Transform>(
                    [&charPosition](ex::Entity, c::UserControl &userControl, c::Transform &transform) {
                        charPosition = transform.getPosition();
                    }
            );

            while (!newItems.empty()) {
                newItems.pop();

                float radius = 1.0f;

                assets::Material *assetMaterial = context->assets->createMaterial("waterball");
                std::shared_ptr<Material> material = assetMaterial->getMaterial();
                std::shared_ptr<Mesh> mesh = Mesh::Create(material);

                GeometryBuilder::SphereDescription sphereDesc;
                sphereDesc.radius = 1.0f;
                GeometryBuilder::Sphere(mesh->geometry, sphereDesc);

                vec3 target = glm::normalize(context->mouseWorldPosition - charPosition);

                float yAxis = glm::clamp(glm::abs(context->mouseWorldPosition.y - charPosition.y), 1.0f, 5.0f);
                float zAxis = glm::clamp(context->mouseWorldPosition.z - charPosition.z, -3.0f, 3.0f);

                game::WorldObject *object = world->createDynamicObject(); // sphere

                world->setObjectMesh(object, mesh);
                world->spawn(object, charPosition + (vec3(0.0f, 1.0f + yAxis, zAxis) + target));
                world->setupRenderMesh(object->getEntity());
//                world->forcePush(object, target, 5.0f)
            }
        }

        void BallShot::receive(const events::KeyPress &event) {
            if (event.key == InputHandler::KEY_E && event.action == 1) {
                newItems.push(1);
            }
        }

        void BallShot::receive(const events::MousePress &event) {
//                if (event.button == 0 && event.action == 1) {
//                    newItems.push(1);
//                }
        }
    }
}