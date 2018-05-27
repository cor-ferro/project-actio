//
// Created by demitriy on 5/27/18.
//

#ifndef ACTIO_DEFAULT_H
#define ACTIO_DEFAULT_H

#include "base.h"

namespace game {
    namespace strategy {
        class WeaponsDefault : public WeaponsBase {
        public:
            WeaponsDefault() : WeaponsBase("Default") {

            };

            ~WeaponsDefault() override = default;

            void onWeaponUpdate(float dt, game::WeaponHandler *weapon) override {}

            void onWeaponFire(game::World *world, game::WeaponHandler *weapon) override {};

            void onWeaponOverheat() override {};

            void onProjectileCreate() override {};

            void onProjectileUpdate(game::World *world, ex::Entity entity, c::WeaponProjectile &projectile) override {};

            void onProjectileCollision(game::World *world, ex::Entity entity, c::WeaponProjectile &projectile) override {};
        protected:

        };
    }
}

#endif //ACTIO_DEFAULT_H
