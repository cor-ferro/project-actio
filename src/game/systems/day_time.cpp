#include "day_time.h"
#include "../world.h"

namespace game {
    namespace systems {
        DayTime::DayTime(World *world) : systems::BaseSystem(world) {}

        void DayTime::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
            time += dt;
            currentDayTime = glm::mod(time, daySize);

            double delta = currentDayTime / daySize;
            float angle = glm::radians(359.9f * static_cast<float>(delta));

            ex::ComponentHandle<components::LightDirectional> dirLight;

            vec3 dir(0.0f, 0.0f, 1.0f);
            quat q = glm::quat(
                    glm::cos(angle / 2.0f),
                    glm::sin(angle / 2.0f) * dir.x,
                    glm::sin(angle / 2.0f) * dir.y,
                    glm::sin(angle / 2.0f) * dir.z
            );

            vec3 newDir = q * vec3(0.0f, 1.0f, 0.0f);

            for (ex::Entity entity : es.entities_with_components(dirLight)) {
                dirLight->setDirection(newDir);
            }
        }

        double DayTime::getTime() {
            return time;
        }

        void DayTime::setDaySize(double newDaySize) {
            daySize = newDaySize;
        }
    }
}