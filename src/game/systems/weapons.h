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
#include "../components/weapon_strategy.h"
#include "../components/weapon_projectile.h"
#include "../events/weapon_action.h"
#include "../events/physic_force.h"
#include "../events/physic_contact.h"
#include "../strategies/weapons/base.h"
#include "../../core/geometry_builder.h"
#include "../context.h"

namespace game {
    struct World;

    namespace systems {
        namespace ex = entityx;
        namespace c = components;

        class Weapons : public systems::BaseSystem, public entityx::System<Weapons>, public ex::Receiver<Weapons> {
        public:
            explicit Weapons(Context& context);

            ~Weapons() override;

            void configure(ex::EventManager &event_manager) override;

            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;

            void create(const std::string &name);

            void receive(const events::PhysicContact &event);

            void receive(const events::Weapon &event);

            void handlePhysicContactData(void *data);

            void processFiredWeapons(ex::EntityManager &es);

            void processWeapons(ex::EntityManager &es, const ex::TimeDelta& dt);

            void processStrategies(ex::EntityManager &es, const ex::TimeDelta& dt);

            void processProjectiles(ex::EntityManager &es, const ex::TimeDelta& dt);

        private:
            std::unordered_map<std::string, strategy::WeaponStrategy *> strategies;

            std::stack<ex::Entity> firedWeapons;

        };
    }
}
#endif //ACTIO_SYSTEMS_WEAPONS_H
