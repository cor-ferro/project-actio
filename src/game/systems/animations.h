//
// Created by demitriy on 4/5/18.
//

#ifndef ACTIO_ANIMATIONS_H
#define ACTIO_ANIMATIONS_H

#include <entityx/entityx/System.h>
#include <entityx/entityx/config.h>
#include <entityx/entityx/Entity.h>
#include <entityx/entityx/Event.h>
#include "../components/skin.h"
#include "../components/model.h"
#include "../events/key_press.h"
#include "../../lib/input_handler.h"
#include "../context.h"
#include "base.h"

namespace game {
    class World;

    namespace systems {
        namespace ex = entityx;

        class Animations
                : public systems::BaseSystem
                  , public ex::System<Animations>
                  , public ex::Receiver<Animations> {
        public:
            explicit Animations(game::World *world);

            void configure(ex::EventManager &event_manager) override;

            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;

            void receive(const events::KeyPress &event);

        private:
            bool isChangeState = false;
            bool isMoving = false;
            bool isJump = false;
        };
    }
}

#endif //ACTIO_ANIMATIONS_H
