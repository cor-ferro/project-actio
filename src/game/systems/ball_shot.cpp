#include "ball_shot.h"

namespace game {
    namespace systems {
        BallShot::BallShot(Context& context) : systems::BaseSystem(context) {}

        void BallShot::configure(ex::EventManager &events) {
            events.subscribe<events::KeyPress>(*this);
        }

        void BallShot::start(ex::EntityManager& es, ex::EventManager& events, ex::TimeDelta dt) {
            systems::BaseSystem::start(es, events, dt);

            ballMaterial = m_context.getAssets()->createMaterial("waterball")->getMaterial();
            ballMesh = m_context.meshes().create();

            GeometryBuilder::SphereDescription sphereDesc;
            sphereDesc.radius = 1.0f;
            sphereDesc.widthSegments = 16;
            sphereDesc.heightSegments = 16;
            GeometryBuilder::Sphere(ballMesh->geometry, sphereDesc);
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

            processQueue<BallDescription>(newBalls, [this, &charPosition, &es, &events](BallDescription &description) {
                vec3 target = glm::normalize(m_context.data.mouseWorldPosition - charPosition);

                float yAxis = glm::clamp(glm::abs(m_context.data.mouseWorldPosition.y - charPosition.y), 1.0f, 5.0f);
                float zAxis = glm::clamp(m_context.data.mouseWorldPosition.z - charPosition.z, -3.0f, 3.0f);

                auto ball = m_context.createObject(es);

                vec3 pos = charPosition + (vec3(0.0f, 1.0f + yAxis, zAxis) + target);
                auto physicActor = m_context.physic().createDynamicActor(pos);
                m_context.physic().createSphereGeometry(physicActor, description.radius);

                ball.assign<components::Mesh>(ballMesh);
                ball.assign<components::PhysicActor>(physicActor);
                ball.assign<components::Renderable>();

                events.emit<events::ObjectCreate>(ball);

                m_context.physic().applyForce(ball, target * description.pushForce);
            });
        }

        void BallShot::receive(const events::KeyPress &event) {
            if (event.key == InputManager::KEY_E && event.action == 1) {
                newBalls.push({});
            }
        }
    }
}