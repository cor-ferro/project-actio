//
// Created by demitriy on 5/9/18.
//

#ifndef ACTIO_STRATEGIES_WEAPONS_BASE_H
#define ACTIO_STRATEGIES_WEAPONS_BASE_H

#include <entityx/entityx/Entity.h>
#include <entityx/entityx/System.h>
#include <entityx/entityx/Event.h>
#include "../../../lib/types.h"
#include "../../../lib/console.h"
#include "../../weapon_handler.h"
#include "../../desc/projectile.h"

namespace game {
    struct World;

    namespace components {
        class WeaponProjectile;
    }

    namespace strategy {
        namespace ex = entityx;
        namespace c = components;

        class WeaponsBase {
        public:
            WeaponsBase(std::string name) : name(name) {

            };

            virtual ~WeaponsBase() = default;

            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {}

            virtual void onWeaponUpdate(float dt, game::WeaponHandler *weaponHandler) = 0;

            virtual void onWeaponFire(game::World *world, game::WeaponHandler *weaponHandler) = 0;

            virtual void onWeaponOverheat() = 0;

            virtual void onProjectileCreate() = 0;

            virtual void onProjectileUpdate(game::World *world, ex::Entity entity, c::WeaponProjectile &projectile) = 0;

            virtual void onProjectileCollision(game::World *world, ex::Entity entity, c::WeaponProjectile &projectile) = 0;

            const std::string getName() {
                return name;
            }

            std::stack<game::desc::WeaponProjectile> &getNewProjectiles() {
                return newProjectiles;
            };

            std::stack<ex::Entity> &getDestroyProjectiles() {
                return destroyProjectiles;
            };

        protected:
            std::string name;

            void createProjectile(game::desc::WeaponProjectile &description) {
                newProjectiles.push(description);
            }

            void destroyProjectile(ex::Entity &entity) {
                destroyProjectiles.push(entity);
            }

        private:
            std::stack<game::desc::WeaponProjectile> newProjectiles;
            std::stack<ex::Entity> destroyProjectiles;
        };
    }
}

#endif //ACTIO_STRATEGIES_WEAPONS_BASE_H
