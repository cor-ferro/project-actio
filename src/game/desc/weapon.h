//
// Created by demitriy on 4/25/18.
//

#ifndef ACTIO_WEAPON_H
#define ACTIO_WEAPON_H

#include <string>

namespace game {
    namespace desc {
        struct Weapon {
            std::string name;

            float maxOverheat = 1.0f;
            float overheatPerShot = 0.1f;
            float weight = 1.0f;
            float fireRate = 1.0f;
        };
    }
}

#endif //ACTIO_WEAPON_H
