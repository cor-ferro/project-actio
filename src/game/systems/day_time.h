//
// Created by demitriy on 4/18/18.
//

#ifndef ACTIO_DAY_TIME_H
#define ACTIO_DAY_TIME_H

#include <entityx/entityx/System.h>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/constants.hpp>
#include "../components/light_directional.h"
#include "../context.h"
#include "base.h"

namespace game {
    class World;

    namespace systems {
        namespace ex = entityx;

        class DayTime
                : public systems::BaseSystem
                  , public entityx::System<DayTime>
                  , public entityx::Receiver<DayTime> {
        public:
            explicit DayTime(World *World);

            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;

            double getTime();

            void setDaySize(double newDaySize);

        private:
            double time = 0.0;
            double currentDayTime = 0.0;
            double daySize = 60000.0;
        };
    }
}

#endif //ACTIO_DAY_TIME_H
