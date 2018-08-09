#ifndef GAME_SYSTEMS_INPUT_H_
#define GAME_SYSTEMS_INPUT_H_

#include <entityx/entityx.h>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <queue>
#include "../events/key_press.h"
#include "../components/transform.h"
#include "../components/camera.h"
#include "../components/target.h"
#include "../../lib/console.h"
#include "../../lib/input_handler.h"
#include "../events/mouse_press.h"
#include "../context.h"
#include "base.h"
#include "../components/user_control.h"
#include "../events/input.h"

namespace game {
    class World;

    namespace systems {
        namespace ex = entityx;
        namespace c = components;

        class Input : public systems::BaseSystem, public ex::System<Input>, public ex::Receiver<Input> {
        public:
            enum InputPlace {
                Input1 = 1,
                Input2 = 2,
                InputAny = 3
            };

            enum Key {
                KEY_SPACE = InputHandler::KEY_SPACE,
                KEY_ENTER = InputHandler::KEY_ENTER,
                KEY_W = InputHandler::KEY_W,
                KEY_A = InputHandler::KEY_A,
                KEY_S = InputHandler::KEY_S,
                KEY_D = InputHandler::KEY_D,
                KEY_C = InputHandler::KEY_C,
                KEY_ESC = InputHandler::KEY_ESC
            };

            enum KeyAction {
                KEY_PRESS = InputHandler::KEY_PRESS,
                KEY_RELEASE = InputHandler::KEY_RELEASE,
                KEY_REPEAT = InputHandler::KEY_REPEAT
            };

            enum MouseButton {
                MOUSE_LEFT = InputHandler::MOUSE_BUTTON_LEFT,
                MOUSE_MIDDLE = InputHandler::MOUSE_BUTTON_MIDDLE,
                MOUSE_RIGHT = InputHandler::MOUSE_BUTTON_RIGHT
            };

            explicit Input(World *world);

            void configure(entityx::EventManager &events) override;

            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;

            void receive(const events::InputSetHandler &event);

            inline bool isPress(InputPlace place, Key key) const;

            inline bool isPress(InputPlace place, MouseButton key) const;

            InputHandler *getHandler(InputPlace place);

        private:
            void configureInput(InputHandler *inputHandler);

            std::queue<events::MousePress> inputMouseQueue;
            std::queue<events::KeyPress> inputKeyboardQueue;
            InputHandler *input1 = nullptr;
            InputHandler *input2 = nullptr;
        };
    }
}

#endif