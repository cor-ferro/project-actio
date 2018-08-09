//
// Created by demitriy on 4/11/18.
//

#ifndef ACTIO_CHARACTER_CONTROL_H
#define ACTIO_CHARACTER_CONTROL_H

#include <entityx/entityx/System.h>
#include <entityx/entityx/config.h>
#include <entityx/entityx/Event.h>
#include <entityx/entityx/Entity.h>
#include <stack>
#include "../events/mouse_press.h"
#include "../events/key_press.h"
#include "../events/setup_controlled.h"
#include "../components/skin.h"
#include "../components/model.h"
#include "../components/transform.h"
#include "../components/controlled.h"
#include "../components/base.h"
#include "../../lib/input_handler.h"
#include "../context.h"
#include "base.h"

namespace game {
    class World;

    namespace systems {
        namespace ex = entityx;

        class CharacterControl
                : systems::BaseSystem
                  , public entityx::System<CharacterControl>
                  , public entityx::Receiver<CharacterControl> {
        public:
            explicit CharacterControl(World *world);

            void configure(ex::EventManager &event_manager) override;

            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;

            void receive(const events::KeyPress &event);

            void receive(const events::SetupControlled &event);

        private:
            struct AnimationCommand {
                int animIndex;
                float weight;
                float step;
            };

            bool isMovingKeyPress = false;
            bool isForwardPress = false;
            bool isBackwardPress = false;
            bool isLeftPress = false;
            bool isRightPress = false;
            bool isJumpPress = false;
            bool isCanJump = false;

            int idleIndex = 0;
            int runIndex = 1;
            int jumpIndex = 2;
            float step = 1.0f / (60.0f * 0.5f);

            bool isControlChange = false;
        };
    }
}

#endif //ACTIO_CHARACTER_CONTROL_H
