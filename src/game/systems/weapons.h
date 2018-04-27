//
// Created by demitriy on 4/25/18.
//

#ifndef ACTIO_SYSTEMS_WEAPONS_H
#define ACTIO_SYSTEMS_WEAPONS_H

#include <entityx/entityx/System.h>
#include <entityx/entityx/Event.h>
#include "base.h"
#include "../desc/weapon.h"
#include "../components/transform.h"
#include "../components/weapon.h"

namespace game {
    namespace systems {
        namespace ex = entityx;
        namespace c = components;

        class Weapons
                : systems::BaseSystem
                  , public entityx::System<Weapons>
                  , public ex::Receiver<Weapons> {
        public:
            explicit Weapons(Context *context) : systems::BaseSystem(context) {}

            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override {
                es.each<c::Weapon>([&dt](ex::Entity entity, c::Weapon &weapon) {
                    weapon.update(static_cast<float>(dt));
                });

                es.each<c::WeaponProjectile, c::Transform>([&dt](
                        ex::Entity entity,
                        c::WeaponProjectile &projectile,
                        c::Transform &transform
                ) {
                    projectile.update(static_cast<float>(dt));
                });
            }

            void registerWeapon(const desc::Weapon &description) {

            }

            void create(const std::string &name) {

            }

        private:
//            std::map<int, Weapon> weapons;
        };
    }
}
#endif //ACTIO_SYSTEMS_WEAPONS_H
