//
// Created by demitriy on 4/25/18.
//

#ifndef ACTIO_COMPONENTS_WEAPON_H
#define ACTIO_COMPONENTS_WEAPON_H

#include <assert.h>
#include <string>
#include <glm/glm/common.hpp>
#include "../../lib/console.h"
#include "../../lib/types.h"
#include "../desc/weapon.h"
#include "../weapon_handler.h"
#include "../desc/projectile.h"

namespace game {
    namespace components {
        struct WeaponProjectile {
            WeaponProjectile(vec3 target) : target(target) {}

            WeaponProjectile(game::desc::WeaponProjectile &description)
                    : maxDamage(description.maxDamage)
                    , weight(description.weight)
                    , impactRadius(description.impactRadius)
                    , damage(description.damage)
                    , speed(description.speed)
                    , health(description.health)
                    , detonationTime(description.detonationTime)
                    , target(description.target)
            {}

            const float maxDamage = 0.0f;
            const float weight = 0.0f;
            const float impactRadius = 0.0f;

            float damage = 0.0f;
            float speed = 0.0f;
            float health = 0.0f;
            float detonationTime = 0.0f;

            vec3 target;
        };

        struct Weapon : WeaponHandler {
            explicit Weapon() = default;

            explicit Weapon(desc::Weapon description) : WeaponHandler(description) {}
        };
    }
}

#endif //ACTIO_COMPONENTS_WEAPON_H
