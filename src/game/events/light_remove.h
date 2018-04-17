//
// Created by demitriy on 3/31/18.
//

#ifndef ACTIO_LIGHT_REMOVE_H
#define ACTIO_LIGHT_REMOVE_H

#include <entityx/entityx/Entity.h>

namespace game {
    namespace events {
        struct LightRemove {
            LightRemove(entityx::Entity entity) : entity(entity) {}

            entityx::Entity entity;
        };
    }
}

#endif //ACTIO_KEY_PRESS_H
