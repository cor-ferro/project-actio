//
// Created by demitriy on 3/31/18.
//

#ifndef ACTIO_PHYSIC_CREATE_H
#define ACTIO_PHYSIC_CREATE_H

#include <entityx/entityx/Entity.h>
#include "../components/physic_entity.h"

namespace game {
    namespace events {
        struct PhysicDesc {
            float denisty = 10.0f;
            float angularDamping = 0.5f;
            std::string material;
        };

        struct PhysicCreateSphere {
            PhysicCreateSphere(entityx::Entity entity, float radius, game::components::PhysicEntity *physicEntity = nullptr)
                    : entity(entity)
                    , physicEntity(physicEntity)
                    , radius(radius) {}

            entityx::Entity entity;
            game::components::PhysicEntity *physicEntity = nullptr;

            float radius;
        };

        struct PhysicCreateBox {
            PhysicCreateBox(entityx::Entity entity, float hx, float hy, float hz, game::components::PhysicEntity *physicEntity = nullptr)
                : entity(entity)
                , physicEntity(physicEntity)
                , hx(hx)
                , hy(hy)
                , hz(hz) {}

            entityx::Entity entity;
            game::components::PhysicEntity *physicEntity = nullptr;

            float hx, hy, hz;
        };
    }
}


#endif //ACTIO_PHYSIC_CREATE_H
