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
#include "../events/weapon_action.h"
#include "../events/physic_create.h"
#include "../events/physic_force.h"

namespace game {
    struct World;

    namespace systems {
        namespace ex = entityx;
        namespace c = components;

        class Weapons
                : systems::BaseSystem
                  , public entityx::System<Weapons>
                  , public ex::Receiver<Weapons> {
        public:
            explicit Weapons(Context *context, game::World *world)
                    : systems::BaseSystem(context)
                    , world(world) {}

            ~Weapons() override {
                for (auto &it : strategies) {
                    strategy::WeaponsBase *strategy = it.second;

                    console::info("destroy weapon system: %s", strategy->getName());

                    delete strategy;
                }

                console::info("weapon system destroyed");
            }

            void configure(ex::EventManager &event_manager) override {
                event_manager.subscribe<events::PhysicContact>(*this);
                event_manager.subscribe<events::Weapon>(*this);
            }

            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override {
                vec3 charPosition, charTarget;

                es.each<components::Model, components::UserControl, components::Transform>(
                        [&charPosition](
                                ex::Entity,
                                components::Model &model,
                                components::UserControl &userControl,
                                components::Transform &transform
                        ) {
                            charPosition = transform.getPosition();
                        });

                while (!firedWeapons.empty()) {
                    ex::Entity entity = firedWeapons.top();

                    ex::ComponentHandle<c::Weapon> weapon = components::get<c::Weapon>(entity);
                    ex::ComponentHandle<c::WeaponStrategy> strategy = components::get<c::WeaponStrategy>(entity);

                    if (strategy) {
                        if (strategy->handler == nullptr) {
                            console::warn("cannot exec weapon, system is empty");
                            return;
                        }

                        auto handler = static_cast<WeaponHandler *>(weapon.get());

                        strategy->handler->onWeaponFire(world, handler);
                    }

                    firedWeapons.pop();
                }

                /**
                 * Update weapons
                 */
                es.each<c::Weapon, c::WeaponStrategy>([&dt](
                        ex::Entity entity,
                        c::Weapon &weapon,
                        c::WeaponStrategy &strategy
                ) {
                    auto handler = static_cast<WeaponHandler*>(&weapon);

                    if (strategy.handler == nullptr) {
                        console::warn("empty weapon system1");
                        return;
                    }

                    strategy.handler->onWeaponUpdate(static_cast<float>(dt), handler);
                });

                /**
                 * Update strategies
                 * @todo: переписать на es.each<c::WeaponStrategy>
                 */
                for (auto &it : strategies) {
                    it.second->update(es, events, dt);

                    std::stack<game::desc::Projectile> &newProjectiles = it.second->getNewProjectiles();

                    while (!newProjectiles.empty()) {
                        strategy::WeaponsBase *strategy = it.second;
                        game::desc::Projectile &projectileDesc = newProjectiles.top();

                        float radius = 0.3f;
                        Mesh *mesh = Mesh::Create();

                        GeometryPrimitive::Sphere(mesh->geometry, radius, 16, 16, 0.0f, glm::two_pi<float>(), 0.0f, 3.14f);
                        mesh->material.setDiffuse(0.0f, 1.0f, 0.0f);

                        ex::Entity projectile = es.create();

                        projectile.assign<c::Model>(mesh);
                        projectile.assign<c::Transform>(projectileDesc.position);
                        projectile.assign<c::WeaponProjectile>(projectileDesc.target);
                        projectile.assign<c::WeaponStrategy>(strategy);

                        events.emit<events::PhysicCreateSphere>(projectile, radius);

                        newProjectiles.pop();
                    }
                }

                /**
                 * Update projectiles
                 */
                es.each<c::WeaponProjectile, c::WeaponStrategy, c::Transform>([&dt, this](
                        ex::Entity entity,
                        c::WeaponProjectile &projectile,
                        c::WeaponStrategy &strategy,
                        c::Transform &transform
                ) {
                    if (strategy.handler == nullptr) {
                        console::warn("empty weapon system2");
                        return;
                    }

                    strategy.handler->onProjectileUpdate(world, entity, projectile);
                });
            }

            bool registerStrategy(strategy::WeaponsBase *strategy) {
                auto it = strategies.find(strategy->getName());

                if (it != strategies.end()) {
                    console::warn("failed already exist weapon system: %s", strategy->getName());
                    return false;
                }

                console::info("register weapon: %s", strategy->getName());

                strategies.insert({ strategy->getName(), strategy });

                return true;
            }

            strategy::WeaponsBase *getWeaponStrategy(const std::string &name) {
                auto it = strategies.find(name);

                if (it == strategies.end()) {
                    it = strategies.find("Default");

                    assert(it != strategies.end() && "not found default weapon system");

                    return nullptr;
                }

                return it->second;
            }

            void create(const std::string &name) {

            }

            void receive(const events::PhysicContact &event) {

            }

            void receive(const events::Weapon &event) {
                switch (event.type) {
                    case events::Weapon::Fire:
                        firedWeapons.push(event.entity);
                        break;
                }
            }

        private:
            std::unordered_map<std::string, strategy::WeaponsBase *> strategies;
            game::World *world = nullptr;

            std::stack<ex::Entity> firedWeapons;

        };
    }
}
#endif //ACTIO_SYSTEMS_WEAPONS_H
