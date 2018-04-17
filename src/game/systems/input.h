#ifndef GAME_SYSTEMS_INPUT_H_
#define GAME_SYSTEMS_INPUT_H_

#include <entityx/entityx.h>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include "../events/key_press.h"
#include "../components/transform.h"
#include "../components/camera.h"
#include "../components/target.h"
#include "../../lib/console.h"
#include "../../lib/input_handler.h"
#include "../events/mouse_press.h"

namespace game {
    namespace systems {
        using namespace entityx;

        class Input : public entityx::System<Input> {
        public:
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
                MOUSE_BUTTON_LEFT = InputHandler::MOUSE_BUTTON_LEFT,
                MOUSE_BUTTON_MIDDLE = InputHandler::MOUSE_BUTTON_MIDDLE,
                MOUSE_BUTTON_RIGHT = InputHandler::MOUSE_BUTTON_RIGHT
            };

            explicit Input(InputHandler *ih) : inputHandler(ih) {}

            void configure(entityx::EventManager &event_manager) {
                inputHandler->subscribeKeyPress([&event_manager](int key, int scancode, int action, int mods) {
                    event_manager.emit<events::KeyPress>(key, scancode, action, mods);
                });

                inputHandler->subscribeMousePress([&event_manager](int button, int action, int mods) {
                    event_manager.emit<events::MousePress>(button, action, mods);
                });
            }

            void update(EntityManager &es, EventManager &events, TimeDelta dt) override {}

        private:
            InputHandler *inputHandler;
        };
    }
}

#endif