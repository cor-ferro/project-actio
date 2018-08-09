#ifndef ACTIO_SYSTEMS_WEAPONS_H
#define ACTIO_SYSTEMS_WEAPONS_H

#include <entityx/entityx/System.h>
#include <entityx/entityx/Event.h>
#include "base.h"
#include "../desc/weapon.h"
#include "../components/base.h"
#include "../components/transform.h"
#include "../components/weapon.h"
#include "../components/model.h"
#include "../components/user_control.h"
#include "../components/weaponStrategy.h"
#include "../events/weapon_action.h"
#include "../events/physic_create.h"
#include "../events/physic_force.h"
#include "../events/physic_contact.h"
#include "../strategies/weapons/base.h"
#include "../../core/geometry_primitive.h"

namespace game {
    struct World;

    namespace systems {
        namespace ex = entityx;
        namespace c = components;

        class Weapons : systems::BaseSystem, public entityx::System<Weapons>, public ex::Receiver<Weapons> {
        public:
            explicit Weapons(World *world);

            ~Weapons() override;

            void configure(ex::EventManager &event_manager) override;

            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;

            bool registerStrategy(strategy::WeaponsBase *strategy);

            strategy::WeaponsBase *getWeaponStrategy(const std::string &name);

            void create(const std::string &name);

            /**
             * Проверяет все возможные события. Необходимо логику проверки типа вынести в систему физики
             * @param event
             */
            void receive(const events::PhysicContact &event);

            void receive(const events::Weapon &event);

            void handlePhysicContactData(void *data);

        protected:
            void createProjectile(game::desc::WeaponProjectile &projectileDesc);

        private:
            std::unordered_map<std::string, strategy::WeaponsBase *> strategies;
            game::World *world = nullptr;

            std::stack<ex::Entity> firedWeapons;

        };
    }
}
#endif //ACTIO_SYSTEMS_WEAPONS_H
