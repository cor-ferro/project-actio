#include "input.h"

namespace game {
    namespace systems {
        Input::Input(Context& context) : systems::BaseSystem(context) {}

        void Input::configure(entityx::EventManager &events) {
            events.subscribe<events::InputSetHandler>(*this);
        }

        void Input::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
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
                m_context.data.mousePosition.x = input1->mouse.x;
                m_context.data.mousePosition.y = input1->mouse.y;

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

        void Input::receive(const events::InputSetHandler &event) {
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

        inline bool Input::isPress(InputPlace place, Key key) const {
            switch (place) {
                case Input1:
                    if (input1 == nullptr) return false;
                    return input1->isPress(static_cast<InputManager::KeyboardKey>(key));
                case Input2:
                    if (input2 == nullptr) return false;
                    return input2->isPress(static_cast<InputManager::KeyboardKey>(key));
                default:
                    console::warn("unknown input place");
                    return false;
            }
        }

        inline bool Input::isPress(InputPlace place, MouseButton key) const {
            switch (place) {
                case Input1:
                    if (input1 == nullptr) return false;
                    return input1->isPress(static_cast<InputManager::MouseButton>(key));
                case Input2:
                    if (input2 == nullptr) return false;
                    return input2->isPress(static_cast<InputManager::MouseButton>(key));
                default:
                    console::warn("unknown input place");
            }
        }

        InputManager *Input::getHandler(InputPlace place) {
            switch (place) {
                case Input1: return input1;
                case Input2: return input2;
                default:
                    return nullptr;
            }
        }

        void Input::configureInput(InputManager *inputHandler) {
//                inputHandler->subscribeKeyPress([&event_manager](int key, int scancode, int action, int mods) {
//                    if (action == InputManager::KEY_REPEAT) return;
//                    event_manager.emit<events::KeyPress>(key, scancode, action, mods);
//                });
//
//                inputHandler->subscribeMousePress([&event_manager](int button, int action, int mods) {
//                    event_manager.emit<events::MousePress>(button, action, mods);
//                });
}
        }
}