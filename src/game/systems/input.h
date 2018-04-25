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
#include "../context.h"
#include "base.h"
#include "../components/user_control.h"

namespace game {
    namespace systems {
        namespace ex = entityx;
        namespace c = components;

        class Input : public systems::BaseSystem
                      , public entityx::System<Input> {
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

            explicit Input(Context *context, InputHandler *ih)
                    : inputHandler(ih)
                    , systems::BaseSystem(context) {}

            void configure(entityx::EventManager &event_manager) {
                inputHandler->subscribeKeyPress([&event_manager](int key, int scancode, int action, int mods) {
                    if (action == InputHandler::KEY_REPEAT) return;
                    event_manager.emit<events::KeyPress>(key, scancode, action, mods);
                });

                inputHandler->subscribeMousePress([&event_manager](int button, int action, int mods) {
                    event_manager.emit<events::MousePress>(button, action, mods);
                });
            }

            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override {
                worldContext->mousePosition.x = inputHandler->mouse.x;
                worldContext->mousePosition.y = inputHandler->mouse.y;

                InputHandler *inputHandler = this->inputHandler;

                es.each<c::UserControl>([&inputHandler](
                        ex::Entity entity,
                        c::UserControl &userControl
                ) {
                    userControl.setLeftPress(inputHandler->isPress(InputHandler::KEY_A));
                    userControl.setRightPress(inputHandler->isPress(InputHandler::KEY_D));
                    userControl.setUpPress(inputHandler->isPress(InputHandler::KEY_W));
                    userControl.setDownPress(inputHandler->isPress(InputHandler::KEY_S));
                    userControl.setJumpPress(inputHandler->isPress(InputHandler::KEY_SPACE));
                    userControl.setIsMainActionPress(inputHandler->isPress(InputHandler::MOUSE_BUTTON_LEFT));
                    userControl.setIsSecondaryActionPress(inputHandler->isPress(InputHandler::MOUSE_BUTTON_RIGHT));
                });
            }

        private:
            InputHandler *inputHandler;
        };
    }
}

#endif