#ifndef GAME_SYSTEMS_MOVEMENT_H_
#define GAME_SYSTEMS_MOVEMENT_H_

#include "entityx/entityx.h"
#include "../components/position.h"
#include "../../lib/console.h"

namespace game {
    namespace systems {
        using namespace entityx;

        class Movement : public entityx::System<Movement> {
        public:
            void update(EntityManager &es, EventManager &events, TimeDelta dt) override {
                
            }
        };
    }
}

#endif