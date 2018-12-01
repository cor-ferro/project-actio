//
// Created by demitriy on 4/28/18.
//

#ifndef ACTIO_WEAPON_FIRE_H
#define ACTIO_WEAPON_FIRE_H

#include <entityx/entityx/Entity.h>

namespace game {
    namespace events {
        namespace ex = entityx;

        struct Weapon {
            enum Type {
                Fire,
                Reload,
                Overheat
            };

            explicit Weapon(Type type, ex::Entity entity) : type(type), entity(entity) {}

            Type type;
            ex::Entity entity;
        };
    }
}

#endif //ACTIO_WEAPON_FIRE_H
