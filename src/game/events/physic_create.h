//
// Created by demitriy on 3/31/18.
//

#ifndef ACTIO_PHYSIC_CREATE_H
#define ACTIO_PHYSIC_CREATE_H

#include <entityx/entityx/Entity.h>

namespace game {
    namespace events {
        struct PhysicCreate {
            PhysicCreate(entityx::Entity entity) : entity(entity) {}

            entityx::Entity entity;
        };
    }
}


#endif //ACTIO_PHYSIC_CREATE_H
