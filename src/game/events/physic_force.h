//
// Created by demitriy on 3/31/18.
//

#ifndef ACTIO_PHYSIC_FORCE_H
#define ACTIO_PHYSIC_FORCE_H

#include <entityx/entityx/Entity.h>
#include "../../lib/types.h"

namespace game {
    namespace events {
        struct PhysicForce {
            PhysicForce(entityx::Entity entity, vec3 direction, float power)
                    : entity(entity)
                    , direction(direction * power) {}

            entityx::Entity entity;
            vec3 direction;
        };
    }
}

#endif //ACTIO_PHYSIC_FORCE_H
