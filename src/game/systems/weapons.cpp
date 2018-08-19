#include "weapons.h"
#include "../world.h"

namespace game {
    namespace systems {
        Weapons::Weapons(game::World *world) : systems::BaseSystem(world) {}

        Weapons::~Weapons() {
            for (auto &it : strategies) {
                strategy::WeaponsBase *strategy = it.second;

                console::info("destroy weapon system: %s", strategy->getName());

                delete strategy;
            }

            console::info("weapon system destroyed");
        }

        void Weapons::configure(ex::EventManager &event_manager) {
            event_manager.subscribe<events::PhysicContact>(*this);
            event_manager.subscribe<events::Weapon>(*this);
        }

        void Weapons::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
            vec3 charPosition, charTarget;

            es.each<components::UserControl, components::Transform>(
                    [&charPosition](
                            ex::Entity,
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

//                    strategy->handler->onWeaponFire(world, handler);
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
                auto handler = static_cast<WeaponHandler *>(&weapon);

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

                std::stack<ex::Entity> &destroyProjectiles = it.second->getDestroyProjectiles();

                while (!destroyProjectiles.empty()) {
                    ex::Entity &entity = destroyProjectiles.top();

                    entity.destroy();

                    destroyProjectiles.pop();
                }

                std::stack<game::desc::WeaponProjectile> &newProjectiles = it.second->getNewProjectiles();

                while (!newProjectiles.empty()) {
                    strategy::WeaponsBase *strategy = it.second;
                    game::desc::WeaponProjectile &projectileDesc = newProjectiles.top();

                    float radius = 0.3f;
                    std::shared_ptr<Mesh> mesh = Mesh::Create();

                    GeometryBuilder::Sphere(mesh->geometry, radius, 16, 16, 0.0f, glm::two_pi<float>(), 0.0f, 3.14f);
                    mesh->material->setDiffuse(0.0f, 1.0f, 0.0f);

                    game::WorldObject *object = world->createDynamicObject();
                    ex::Entity entity = object->getEntity();

//                    entity.assign<c::PhysicEntity>(c::PhysicEntity::WeaponProjectile, projectile);
                    entity.assign<c::WeaponProjectile>(projectileDesc);
                    entity.assign<c::WeaponStrategy>(strategy);

                    world->setObjectMesh(object, mesh);
                    world->spawn(object, projectileDesc.position);

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

        bool Weapons::registerStrategy(strategy::WeaponsBase *strategy) {
            auto it = strategies.find(strategy->getName());

            if (it != strategies.end()) {
                console::warn("failed already exist weapon system: %s", strategy->getName());
                return false;
            }

            console::info("register weapon: %s", strategy->getName());

            strategies.insert({strategy->getName(), strategy});

            return true;
        }

        strategy::WeaponsBase *Weapons::getWeaponStrategy(const std::string &name) {
            auto it = strategies.find(name);

            if (it == strategies.end()) {
                it = strategies.find("Default");

                assert(it != strategies.end() && "not found default weapon system");

                return nullptr;
            }

            return it->second;
        }

        void Weapons::create(const std::string &name) {

        }

        /**
         * Проверяет все возможные события. Необходимо логику проверки типа вынести в систему физики
         * @param event
         */
        void Weapons::receive(const events::PhysicContact &event) {
            if (event.actor1->userData != nullptr) {
                handlePhysicContactData(event.actor1->userData);
            }

            if (event.actor2->userData != nullptr) {
                handlePhysicContactData(event.actor2->userData);
            }

        }

        void Weapons::receive(const events::Weapon &event) {
            switch (event.type) {
                case events::Weapon::Fire:
                    firedWeapons.push(event.entity);
                    break;
            }
        }

        void Weapons::handlePhysicContactData(void *data) {
            auto *physicEntity = static_cast<c::PhysicEntity *>(data);

            switch (physicEntity->type) {
                case c::PhysicEntity::WeaponProjectile:
                    auto projectile = components::get<c::WeaponProjectile>(physicEntity->entity);
                    auto strategy = components::get<c::WeaponStrategy>(physicEntity->entity);

                    strategy->handler->onProjectileCollision(world, physicEntity->entity, *projectile);

                    break;
            }
        }

        void Weapons::createProjectile(game::desc::WeaponProjectile &projectileDesc) {

        }
    }
}