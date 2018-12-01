#include "weapons.h"

namespace game {
    namespace systems {
        Weapons::Weapons(Context& context) : systems::BaseSystem(context) {}

        Weapons::~Weapons() {
            for (auto &it : strategies) {
                strategy::WeaponStrategy *strategy = it.second;

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
            processFiredWeapons(es);

            processWeapons(es, dt);

            processStrategies(es, dt);

            processProjectiles(es, dt);
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

        void Weapons::processFiredWeapons(ex::EntityManager& es) {
            while (!firedWeapons.empty()) {
                ex::Entity entity = firedWeapons.top();

                ex::ComponentHandle<c::Weapon> weapon = components::get<c::Weapon>(entity);
                ex::ComponentHandle<c::WeaponStrategy> strategy = components::get<c::WeaponStrategy>(entity);

                if (strategy) {
                    if (strategy->handler == nullptr) {
                        console::warn("cannot exec weapon, system is empty");
                        return;
                    }

                    auto handler = static_cast<Weapon *>(weapon.get());

//                    strategy->handler->onWeaponFire(world, handler);
                }

                firedWeapons.pop();
            }
        }

        void Weapons::processWeapons(ex::EntityManager& es, const ex::TimeDelta& dt) {
            es.each<c::Weapon, c::WeaponStrategy>([&dt](
                    ex::Entity entity,
                    c::Weapon &weapon,
                    c::WeaponStrategy &strategy
            ) {
                auto handler = static_cast<Weapon *>(&weapon);

                if (strategy.handler == nullptr) {
                    console::warn("empty weapon system1");
                    return;
                }

                strategy.handler->onWeaponUpdate(static_cast<float>(dt), handler);
            });
        }

        void Weapons::processStrategies(ex::EntityManager& es, const ex::TimeDelta& dt) {
            // @todo: переписать на es.each<c::WeaponStrategy>
            for (auto &it : strategies) {

                std::stack<ex::Entity> &destroyProjectiles = it.second->getDestroyProjectiles();

                while (!destroyProjectiles.empty()) {
                    ex::Entity &entity = destroyProjectiles.top();

                    entity.destroy();

                    destroyProjectiles.pop();
                }

                std::stack<desc::WeaponProjectile> &newProjectiles = it.second->getNewProjectiles();

                strategy::WeaponStrategy *strategy = it.second;

                while (!newProjectiles.empty()) {
                    desc::WeaponProjectile &description = newProjectiles.top();

                    ex::Entity entity = es.create();

                    entity.assign<c::WeaponStrategy>(strategy);
                    entity.assign<c::WeaponProjectile>(description);

                    newProjectiles.pop();

                    strategy->onProjectileCreate(/*entity*/);

//                    strategy::WeaponStrategy *strategy = it.second;
//                    game::desc::WeaponProjectile &projectileDesc = newProjectiles.top();
//
//                    float radius = 0.3f;
//                    std::shared_ptr<Mesh> mesh = Mesh::Create();
//
//                    GeometryBuilder::SphereDescription desc;
//                    desc.radius = radius;
//                    desc.widthSegments = 16;
//                    desc.heightSegments = 16;
//                    GeometryBuilder::Sphere(mesh->geometry, desc);
//                    mesh->material->setDiffuse(0.0f, 1.0f, 0.0f);
//
//                    game::WorldObject *object = world->createDynamicObject();
//                    ex::Entity entity = object->getEntity();
//
////                    entity.assign<c::PhysicEntity>(c::PhysicEntity::WeaponProjectile, projectile);
//                    entity.assign<c::WeaponProjectile>(projectileDesc);
//                    entity.assign<c::WeaponStrategy>(strategy);
//
//                    world->setObjectMesh(object, mesh);
//                    world->spawn(object, projectileDesc.position);
//

                }
            }
        }

        void Weapons::processProjectiles(ex::EntityManager& es, const ex::TimeDelta& dt) {
            es.each<c::WeaponProjectile, c::WeaponStrategy, c::Transform>([](
                    ex::Entity entity,
                    c::WeaponProjectile &projectile,
                    c::WeaponStrategy &strategy,
                    c::Transform &transform
            ) {
                if (strategy.handler == nullptr) {
                    console::warn("empty weapon system2");
                    return;
                }

                strategy.handler->onProjectileUpdate(entity, projectile);
            });
        }

        void Weapons::handlePhysicContactData(void *data) {
//            auto *physicEntity = static_cast<components::PhysicEntity *>(data);
//
//            switch (physicEntity->type) {
//                case c::PhysicEntity::WeaponProjectile:
//                    auto projectile = components::get<c::WeaponProjectile>(physicEntity->entity);
//                    auto strategy = components::get<c::WeaponStrategy>(physicEntity->entity);
//
//                    strategy->handler->onProjectileCollision(world, physicEntity->entity, *projectile);
//
//                    break;
//            }
        }
    }
}