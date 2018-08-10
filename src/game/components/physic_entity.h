//
// Created by demitriy on 5/27/18.
//

#ifndef ACTIO_COMPONENTS_PHYSIC_ENTITY_H
#define ACTIO_COMPONENTS_PHYSIC_ENTITY_H

#include <entityx/entityx.h>

namespace game {
    namespace components {
        namespace ex = entityx;

        struct PhysicEntity {
            enum Type {
                Character,
                Weapon,
                WeaponProjectile,
                StaticObject,
                DynamicObject,
                Other
            };

            Type type;
            ex::Entity entity;

            PhysicEntity() : type(Other) {}

            PhysicEntity(Type type) : type(type) {}
            PhysicEntity(Type type, ex::Entity entity) : type(type), entity(entity) {} // @todo: remove entity reference
        };
    }
}

#endif //ACTIO_COMPONENTS_PHYSIC_ENTITY_H
