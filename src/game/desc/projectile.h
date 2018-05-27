//
// Created by demitriy on 4/25/18.
//

#ifndef ACTIO_DESC_PROJECTILE_H
#define ACTIO_DESC_PROJECTILE_H

#include <string>
#include "../world.h"
#include "../strategies/weapons/base.h"

namespace game {
    namespace desc {
        struct Projectile {
            Projectile() = default;

            vec3 position;
            vec3 direction;
            vec3 target;
        };
    }
}

#endif //ACTIO_DESC_PROJECTILE_H
