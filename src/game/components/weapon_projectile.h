//
// Created by demitriy on 11/8/18.
//

#ifndef ACTIO_WEAPON_PROJECTILE_H
#define ACTIO_WEAPON_PROJECTILE_H

#include "../../lib/types.h"
#include "../desc/projectile.h"

namespace game {
    namespace components {
        struct WeaponProjectile {
            explicit WeaponProjectile(const vec3& target) : target(target) {}

            explicit WeaponProjectile(game::desc::WeaponProjectile& description)
                    : maxDamage(description.maxDamage)
                    , weight(description.weight)
                    , impactRadius(description.impactRadius)
                    , damage(description.damage)
                    , speed(description.speed)
                    , health(description.health)
                    , detonationTime(description.detonationTime)
                    , target(description.target) {}

            const float maxDamage = 0.0f;
            const float weight = 0.0f;
            const float impactRadius = 0.0f;

            float damage = 0.0f;
            float speed = 0.0f;
            float health = 0.0f;
            float detonationTime = 0.0f;

            vec3 target;
        };
    }
}

#endif //ACTIO_WEAPON_PROJECTILE_H
