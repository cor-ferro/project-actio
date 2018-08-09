//
// Created by demitriy on 4/20/18.
//

#ifndef ACTIO_BASE_SYSTEM_H
#define ACTIO_BASE_SYSTEM_H

#include <entityx/entityx/entityx.h>
#include "../context.h"

namespace game {
    class World;

    namespace systems {
        namespace ex = entityx;

        class BaseSystem {
        public:
            explicit BaseSystem(game::World *world);

        protected:
            game::World *world = nullptr;
            game::Context *context = nullptr;
        };
    }
}

#endif //ACTIO_BASE_SYSTEM_H
