//
// Created by demitriy on 4/18/18.
//

#ifndef ACTIO_DAY_TIME_H
#define ACTIO_DAY_TIME_H

#include <entityx/entityx/System.h>
#include "../components/light_directional.h"
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/constants.hpp>

namespace game {
    namespace systems {
        using namespace entityx;

        class DayTime : public entityx::System<DayTime>, public entityx::Receiver<DayTime> {
        public:
            void update(EntityManager &es, EventManager &events, TimeDelta dt) override {
                time+= dt;
                currentDayTime = glm::mod(time, daySize);

                double delta = currentDayTime / daySize;
                float angle = glm::radians(359.9f * static_cast<float>(delta));

                ComponentHandle<components::LightDirectional> dirLight;

                vec3 dir(0.0f, 0.0f, 1.0f);
                quat q = glm::quat(
                        glm::cos(angle / 2.0f),
                        glm::sin(angle / 2.0f) * dir.x,
                        glm::sin(angle / 2.0f) * dir.y,
                        glm::sin(angle / 2.0f) * dir.z
                );

                vec3 newDir = q * vec3(0.0f, 1.0f, 0.0f);

                for (Entity entity : es.entities_with_components(dirLight)) {
                    dirLight->setDirection(newDir);
                }
            }

            double getTime() {
                return time;
            }

            void setDaySize(double newDaySize) {
                daySize = newDaySize;
            }
        private:

            double time = 0.0;
            double currentDayTime = 0.0;
            double daySize = 60000.0;
        };
    }
}

#endif //ACTIO_DAY_TIME_H
