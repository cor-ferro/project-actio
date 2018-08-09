#include "sky.h"
#include "../world.h"

namespace game {
    namespace systems {
        Sky::Sky(World *world) : systems::BaseSystem(world) {}

        void Sky::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
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
    }
}