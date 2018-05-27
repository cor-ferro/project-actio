//
// Created by demitriy on 5/9/18.
//

#include "base.h"
#include "../../../lib/types.h"

#ifndef ACTIO_ROCKET_LAUNCHER_H
#define ACTIO_ROCKET_LAUNCHER_H

namespace game {
    namespace strategy {
        class WeaponsRocketLauncher : public WeaponsBase {
        public:
            WeaponsRocketLauncher() : WeaponsBase("RocketLauncher") {

            }

            ~WeaponsRocketLauncher() override = default;

            /**
             * Метод обновления оружия. Выполняется на каждой итерации.
             *
             * Автоматически снижаем уровень перегрева на каждой итерации.
             *
             * @param weapon
             */
            void onWeaponUpdate(float dt, game::WeaponHandler *weapon) override {
//                console::info("rocket launcher update");
//                weapon->tickRecoil();
            }

            /**
             * Методы для действия выстрела оружия.
             *
             * При каждом выстреле повышать уровень перегрева
             *
             * @param weapon
             */
            void onWeaponFire(game::World *world, game::WeaponHandler *weapon) override {
                console::info("rocket launcher fire");

                const game::Context &context = world->getContext();
                World::Character character = world->getUserControlCharacter();

                float fireStability = weapon->fireStability();

                vec3 charPosition = character.transform->getPosition();

                vec3 direction(0.0f); // Рандом умножать на стабильность огня. Чем выше стабильность, тем меньше разброс
                vec3 target = glm::normalize(context.mouseWorldPosition - charPosition);
                float yAxis = glm::clamp(glm::abs(context.mouseWorldPosition.y - charPosition.y), 1.0f, 5.0f);
                float zAxis = glm::clamp(context.mouseWorldPosition.z - charPosition.z, -3.0f, 3.0f);

                vec3 position(charPosition + (vec3(0.0f, 1.0f + yAxis, zAxis) + target));

                /*
                 позиция
                    простой вариант: персонаж + некое расстоние
                    сложный вариант: bbox оружия + некое расстояние
                 направление
                 цель (используется в случае наличия функционала автонаведения), если цели нет, то просто направление плюсуем
                 скорость замедления (или применение силы для ракет)
                 и стандартное описание шелла для данного типа оружия: урон и т.п.
                 */

//                weapon->increaseOverheat();
//                weapon->increaseFireRate();

                game::desc::Projectile projectileDescription;
                projectileDescription.position = position;
                projectileDescription.target = target;
                projectileDescription.direction = direction;

                createProjectile(projectileDescription);
            };

            void onWeaponOverheat() override {};

            void onProjectileCreate() override {};

            void onProjectileUpdate(game::World *world, ex::Entity entity, c::WeaponProjectile &projectile) override {
                world->forcePush(entity, projectile.target, 1.0f);
            };

            void onProjectileCollision(game::World *world, ex::Entity entity, c::WeaponProjectile &projectile) override {
                projectile.health-= 0.9f;

                if (projectile.health < 0.0f) {
                    // destroy projectile
                }
            };
        };
    }
}

#endif //ACTIO_ROCKET_LAUNCHER_H
