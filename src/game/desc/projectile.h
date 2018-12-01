//
// Created by demitriy on 4/25/18.
//

#ifndef ACTIO_DESC_PROJECTILE_H
#define ACTIO_DESC_PROJECTILE_H

#include <string>

namespace game {
    namespace desc {
        struct WeaponProjectile {
            WeaponProjectile() = default;

            vec3 position;
            vec3 direction;
            vec3 target;

            float maxDamage = 1.0f;
            float weight = 1.0f;
            float impactRadius = 1.0f;
            float damage = 1.0f;
            float speed = 1.0f;
            float health = 1.0f;
            float detonationTime = 1.0f;
        };
    }
}

#endif //ACTIO_DESC_PROJECTILE_H
