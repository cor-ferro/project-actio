//
// Created by demitriy on 4/28/18.
//

#ifndef ACTIO_WEAPON_HANDLER_H
#define ACTIO_WEAPON_HANDLER_H

#include <glm/glm.hpp>
#include "desc/weapon.h"

namespace game {
    struct WeaponFireMode {

    };

    class WeaponHandler {
    public:
        WeaponHandler(desc::Weapon &description)
                : name(description.name)
                , weight(description.weight)
                , overheatPerShot(description.overheatPerShot)
                , maxOverheat(description.maxOverheat)
                , fireRate(description.fireRate)
        {}

        float overheatRate() {
            assert(overheat != 0.0f && maxOverheat != 0.0f);

            return overheat / maxOverheat;
        }

        bool isOverheated() {
            return overheat >= maxOverheat;
        }

        bool canFire() {
            return fireRate <= maxFireRate;
        }

        float fireStability() {
            return fireRate / maxFireRate;
        }

        void tickRecoil() {
            if (overheat > 0.0f) {
                overheat = glm::clamp(overheat - recoil, 0.0f, maxOverheat);
            }
        }

        void fullRecoil() {
            overheat = 0.0f;
        }

        void increaseOverheat() {
            overheat+= overheatPerShot;
        }

        void increaseFireRate() {
            fireRate+= 1;
        }

        WeaponFireMode fireMode;

        const float maxOverheat = 0.0f;
        const float overheatPerShot = 0.0f;
        const float weight = 0.0f;
        const float maxFireRate = 0;

        float overheat = 0.0f;
        float recoil = 0.0f;
        float fireRate = 0;

        std::string name;
    private:
    };
}

#endif //ACTIO_WEAPON_HANDLER_H
