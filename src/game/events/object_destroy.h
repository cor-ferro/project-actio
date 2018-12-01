//
// Created by demitriy on 11/13/18.
//

#ifndef ACTIO_OBJECT_DESTROY_H
#define ACTIO_OBJECT_DESTROY_H

#include <entityx/Entity.h>

namespace game {
    namespace events {
        struct ObjectDestroy {
            explicit ObjectDestroy(entityx::Entity& entity) : entity(entity) {}

            entityx::Entity entity;
        };
    }
}

#endif //ACTIO_OBJECT_DESTROY_H
