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
#include "../../lib/input_manager.h"
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
            explicit Animations(Context& context);

            void configure(ex::EventManager &event_manager) override;

            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;
        };
    }
}

#endif //ACTIO_ANIMATIONS_H
