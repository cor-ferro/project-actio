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

namespace game {
    namespace components {
        struct WeaponFireMode {

        };

        struct WeaponProjectile {
            void update(float dt) {

            }

            const float maxDamage = 0.0f;
            const float weight = 0.0f;
            const float impactRadius = 0.0f;

            float damage = 0.0f;
            float speed = 0.0f;
            float health = 0.0f;
            float detonationTime = 0.0f;
        };

        struct Weapon {
            explicit Weapon() = default;

            explicit Weapon(desc::Weapon description)
                : name(description.name)
                , weight(description.weight)
                , overheatPerShot(description.overheatPerShot)
                , maxOverheat(description.maxOverheat)
                , fireRate(description.fireRate)
            {
                name = description.name;
            }

            void update(float dt) {
                if (fireRate > 0) {
                    fireRate-= 1;
                }
            }

            void fire(vec3 target) {
                if (!canFire()) {
                    return;
                }

                if (isOverheated()) {

                }

                increaseOverheat();
                increaseFireRate();

                console::geom("fire %s", target);
            }

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

            WeaponFireMode fireMode;
            WeaponProjectile projectile;

            const float maxOverheat = 0.0f;
            const float overheatPerShot = 0.0f;
            const float weight = 0.0f;
            const float maxFireRate = 0;

            float overheat = 0.0f;
            float recoil = 0.0f;
            float fireRate = 0;

            std::string name;
        private:
            void tickRecoil() {
                if (overheat > 0.0f) {
                    overheat = glm::clamp(overheat - recoil, 0.0f, maxOverheat);
                }
            }

            void increaseOverheat() {
                overheat+= overheatPerShot;
            }

            void increaseFireRate() {
                fireRate+= 1;
            }
        };


    }
}

#endif //ACTIO_COMPONENTS_WEAPON_H
