#ifndef ACTIO_GAME_HELPER_H
#define ACTIO_GAME_HELPER_H

#include <entityx/entityx/entityx.h>

namespace game {
    namespace components {
        using namespace entityx;

        struct Helper {
            Helper(entityx::Entity entity) : entity(entity) {}

            entityx::Entity entity;
        };
    }
}

#endif //ACTIO_LIGHT_HELPER_H
