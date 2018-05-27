//
// Created by demitriy on 4/20/18.
//

#ifndef ACTIO_BASE_SYSTEM_H
#define ACTIO_BASE_SYSTEM_H

#include <entityx/entityx/entityx.h>
#include "../context.h"
#include "../world.h"

namespace game {
    struct World;

    namespace systems {
        namespace ex = entityx;

        class BaseSystem {
        public:
            explicit BaseSystem(game::Context *context) : worldContext(context) {}

        protected:
            game::Context *worldContext = nullptr;
        };
    }
}

#endif //ACTIO_BASE_SYSTEM_H
