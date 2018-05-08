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
    namespace systems {
        namespace ex = entityx;
        namespace c = components;

        class Input : public systems::BaseSystem
                      , public ex::System<Input>
                      , public ex::Receiver<Input> {
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

            explicit Input(Context *context) : systems::BaseSystem(context) {}

            void configure(entityx::EventManager &events) {
                events.subscribe<events::InputSetHandler>(*this);
            }

            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override {
                while (!inputMouseQueue.empty()) {
                    events::MousePress &event = inputMouseQueue.front();

                    events.emit<events::MousePress>(event);

                    inputMouseQueue.pop();
                }

                while (!inputKeyboardQueue.empty()) {
                    events::KeyPress &event = inputKeyboardQueue.front();

                    events.emit<events::KeyPress>(event);

                    inputKeyboardQueue.pop();
                }

                if (input1 != nullptr) {
                    worldContext->mousePosition.x = input1->mouse.x;
                    worldContext->mousePosition.y = input1->mouse.y;

                    es.each<c::UserControl>([&](ex::Entity entity, c::UserControl &userControl) {
                        userControl.setLeftPress(isPress(Input1, KEY_A));
                        userControl.setRightPress(isPress(Input1, KEY_D));
                        userControl.setUpPress(isPress(Input1, KEY_W));
                        userControl.setDownPress(isPress(Input1, KEY_S));
                        userControl.setJumpPress(isPress(Input1, KEY_SPACE));
                        userControl.setIsMainActionPress(isPress(Input1, MOUSE_LEFT));
                        userControl.setIsSecondaryActionPress(isPress(Input1, MOUSE_RIGHT));
                    });
                }
            }

            void receive(const events::InputSetHandler &event) {
                switch (event.place) {
                    case 1: input1 = event.inputHandler; break;
                    case 2: input2 = event.inputHandler; break;
                    default: console::warn("unknown place");
                }

                std::queue<events::MousePress> *inputMouseQueue = &this->inputMouseQueue;
                std::queue<events::KeyPress> *inputKeyboardQueue = &this->inputKeyboardQueue;

                event.inputHandler->subscribeMousePress([inputMouseQueue](int button, int action, int mods) {
                    inputMouseQueue->push(events::MousePress(button, action, mods));
                });

                event.inputHandler->subscribeKeyPress([inputKeyboardQueue](int key, int scancode, int action, int mods) {
                    inputKeyboardQueue->push(events::KeyPress(key, scancode, action, mods));
                });
            }

            inline bool isPress(InputPlace place, Key key) const {
                switch (place) {
                    case Input1:
                        if (input1 == nullptr) return false;
                        return input1->isPress(static_cast<InputHandler::KeyboardKey>(key));
                    case Input2:
                        if (input2 == nullptr) return false;
                        return input2->isPress(static_cast<InputHandler::KeyboardKey>(key));
                    default:
                        console::warn("unknown input place");
                }
            }

            inline bool isPress(InputPlace place, MouseButton key) const {
                switch (place) {
                    case Input1:
                        if (input1 == nullptr) return false;
                        return input1->isPress(static_cast<InputHandler::MouseButton>(key));
                    case Input2:
                        if (input2 == nullptr) return false;
                        return input2->isPress(static_cast<InputHandler::MouseButton>(key));
                    default:
                        console::warn("unknown input place");
                }
            }

            InputHandler *getHandler(InputPlace place) {
                switch (place) {
                    case Input1: return input1;
                    case Input2: return input2;
                    default:
                        return nullptr;
                }
            }

        private:
            void configureInput(InputHandler *inputHandler) {
//                inputHandler->subscribeKeyPress([&event_manager](int key, int scancode, int action, int mods) {
//                    if (action == InputHandler::KEY_REPEAT) return;
//                    event_manager.emit<events::KeyPress>(key, scancode, action, mods);
//                });
//
//                inputHandler->subscribeMousePress([&event_manager](int button, int action, int mods) {
//                    event_manager.emit<events::MousePress>(button, action, mods);
//                });
            }

            std::queue<events::MousePress> inputMouseQueue;
            std::queue<events::KeyPress> inputKeyboardQueue;

            InputHandler *input1 = nullptr;
            InputHandler *input2 = nullptr;
        };
    }
}

#endif