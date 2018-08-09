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
            float charHeight = 0.0f;

            es.each<components::Camera>(
                    [&cameraPosition, &cameraTarget](ex::Entity entity, components::Camera &camera) {
                        cameraPosition = camera.getPosition();
                        cameraTarget = camera.getTarget();
                    });

            es.each<components::Model, c::UserControl, components::Transform>(
                    [&charPosition, &charHeight](
                            ex::Entity,
                            c::Model &model,
                            c::UserControl &userControl,
                            c::Transform &transform
                    ) {
                        charPosition = transform.getPosition();
                        charHeight = model.height() * transform.getScale().y;
                    });

            while (!newItems.empty()) {
                newItems.pop();

                ex::Entity ball = es.create();

                float radius = 1.0f;

//                    ex::Entity ball2 = context->createDynamicEntity();
//
//                    ball2.assign<c::Model>(missileModel);
//                    ball2.assign<c::Transform>(vec3(1.0f));

//                    context->createDynamicObject(assetModel);

//                    assets::Model *assetModel = context->assets->findModel("missile");
//                    if (assetModel != nullptr) {
////                        ball.assign<components::Model>(mesh);
//
//                    }

                assets::Material *assetMaterial = context->assets->createMaterial("waterball");
                std::shared_ptr<Material> material = assetMaterial->getMaterial();
                Mesh *mesh = Mesh::Create(material);

                GeometryPrimitive::Sphere(mesh->geometry, radius, 16, 16, 0.0f, glm::two_pi<float>(), 0.0f, 3.14f);

                vec3 target = glm::normalize(context->mouseWorldPosition - charPosition);

                float yAxis = glm::clamp(glm::abs(context->mouseWorldPosition.y - charPosition.y), 1.0f, 5.0f);
                float zAxis = glm::clamp(context->mouseWorldPosition.z - charPosition.z, -3.0f, 3.0f);

                ball.assign<components::Model>(mesh);
                ball.assign<components::Transform>(charPosition + (vec3(0.0f, 1.0f + yAxis, zAxis) + target));

                events.emit<events::PhysicCreateSphere>(ball, radius);
                events.emit<events::PhysicForce>(ball, target, 5.0f);
                events.emit<events::RenderSetupModel>(ball);
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