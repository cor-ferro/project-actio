#ifndef ACTIO_GAME_LIGHT_HELPER_H
#define ACTIO_GAME_LIGHT_HELPER_H

#include <entityx/entityx/entityx.h>

namespace game {
    namespace components {
        namespace ex = entityx;

        struct LightHelper {
            LightHelper(ex::Entity entity) : entity(entity) {}

            ex::Entity entity;
        };
    }
}

#endif //ACTIO_LIGHT_HELPER_H
