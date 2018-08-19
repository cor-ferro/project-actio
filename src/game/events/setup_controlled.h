//
// Created by demitriy on 4/5/18.
//

#ifndef ACTIO_MAKE_CONTROLLED_H
#define ACTIO_MAKE_CONTROLLED_H

#include <entityx/entityx/Entity.h>

namespace game {
    namespace events {
        struct MakeControlled {
            explicit MakeControlled(const entityx::Entity &entity) : entity(entity) {}

            entityx::Entity entity;
        };

        struct MakeUnControlled {
            explicit MakeUnControlled(const entityx::Entity &entity) : entity(entity) {}

            entityx::Entity entity;
        };
    }
}

#endif //ACTIO_MAKE_CONTROLLED_H
