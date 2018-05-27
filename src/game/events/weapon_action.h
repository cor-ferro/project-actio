//
// Created by demitriy on 4/28/18.
//

#ifndef ACTIO_WEAPON_FIRE_H
#define ACTIO_WEAPON_FIRE_H

namespace game {
    namespace events {
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
