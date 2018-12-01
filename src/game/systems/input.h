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
#include "../../lib/input_manager.h"
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
                KEY_SPACE = InputManager::KEY_SPACE,
                KEY_ENTER = InputManager::KEY_ENTER,
                KEY_W = InputManager::KEY_W,
                KEY_A = InputManager::KEY_A,
                KEY_S = InputManager::KEY_S,
                KEY_D = InputManager::KEY_D,
                KEY_C = InputManager::KEY_C,
                KEY_ESC = InputManager::KEY_ESC
            };

            enum KeyAction {
                KEY_PRESS = InputManager::KEY_PRESS,
                KEY_RELEASE = InputManager::KEY_RELEASE,
                KEY_REPEAT = InputManager::KEY_REPEAT
            };

            enum MouseButton {
                MOUSE_LEFT = InputManager::MOUSE_BUTTON_LEFT,
                MOUSE_MIDDLE = InputManager::MOUSE_BUTTON_MIDDLE,
                MOUSE_RIGHT = InputManager::MOUSE_BUTTON_RIGHT
            };

            explicit Input(Context& context);

            void configure(entityx::EventManager &events) override;

            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;

            void receive(const events::InputSetHandler &event);

            inline bool isPress(InputPlace place, Key key) const;

            inline bool isPress(InputPlace place, MouseButton key) const;

            InputManager *getHandler(InputPlace place);

        private:
            void configureInput(InputManager *inputHandler);

            std::queue<events::MousePress> inputMouseQueue;
            std::queue<events::KeyPress> inputKeyboardQueue;
            InputManager *input1 = nullptr;
            InputManager *input2 = nullptr;
        };
    }
}

#endif