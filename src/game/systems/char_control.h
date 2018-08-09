//
// Created by demitriy on 4/11/18.
//

#ifndef ACTIO_CHAR_CONTROL_H
#define ACTIO_CHAR_CONTROL_H

#include <entityx/entityx/System.h>
#include <entityx/entityx/config.h>
#include <entityx/entityx/Event.h>
#include <entityx/entityx/Entity.h>
#include <stack>
#include "../events/mouse_press.h"
#include "../events/key_press.h"
#include "../events/setup_controlled.h"
#include "../events/weapon_action.h"
#include "../components/skin.h"
#include "../components/model.h"
#include "../components/base.h"
#include "../components/transform.h"
#include "../components/character.h"
#include "../components/char_items.h"
#include "../components/user_control.h"
#include "../components/ai_control.h"
#include "../../lib/input_handler.h"
#include "../context.h"
#include "base.h"

namespace game {
    class World;

    namespace systems {
        namespace ex = entityx;
        namespace c = components;

        class CharControl
                : systems::BaseSystem
                  , public entityx::System<CharControl>
                  , public ex::Receiver<CharControl> {
        public:
            enum ExecType {
                ExecWeapon
            };

            explicit CharControl(World *world);

            void configure(ex::EntityManager &es, entityx::EventManager &events) override;

            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;

            void receive(const events::MousePress &event);

            void execItem(ex::Entity activeItem, vec3 mousePos);

        private:
            void handleUserControl() {}

            void handleAiControl() {}

            ex::EntityManager *entityManager = nullptr;

            std::stack<std::pair<ExecType, ex::Entity>> newExec;
        };
    }
}

#endif // ACTIO_CHAR_CONTROL_H
