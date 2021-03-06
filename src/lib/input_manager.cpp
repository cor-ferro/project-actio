#include "input_manager.h"

InputManager::InputManager() {
    onKeyPress.connect([this](int key, int scancode, int action, int mods) {
        bool keyState = false;

        switch (action) {
            case KEY_PRESS:     keyState = true; break;
            case KEY_REPEAT:    keyState = true; break;
            case KEY_RELEASE:   keyState = false; break;
            default: console::warn("unknown keyboard action");
        }

        keyboardKeys_.set(static_cast<size_t>(key), keyState);

        switch (mods) {
            case MODIFIER_SHIFT:    keyboardModifiers_.set(MODIFIER_SHIFT, true); break;
            case MODIFIER_CONTROL:  keyboardModifiers_.set(MODIFIER_CONTROL, true); break;
            case MODIFIER_ALT:      keyboardModifiers_.set(MODIFIER_ALT, true); break;
            case MODIFIER_SUPER:    keyboardModifiers_.set(MODIFIER_SUPER, true); break;
            default: keyboardModifiers_.reset();
        }
    });

    onMouseMove.connect([this](double x, double y) {
        setMousePosition(static_cast<ScreenCoord>(x), static_cast<ScreenCoord>(y));
    });

    onMousePress.connect([this](int button, int action, int mods) {
        bool state = false;

        switch (action) {
            case KEY_PRESS:     state = true; break;
            case KEY_RELEASE:   state = false; break;
            default: console::warn("unknown mouse action");
        }

        switch (button) {
            case MOUSE_BUTTON_LEFT:     mouseKeys_.set(MouseButton::MOUSE_BUTTON_LEFT, state); break;
            case MOUSE_BUTTON_MIDDLE:   mouseKeys_.set(MouseButton::MOUSE_BUTTON_MIDDLE, state); break;
            case MOUSE_BUTTON_RIGHT:    mouseKeys_.set(MouseButton::MOUSE_BUTTON_RIGHT, state); break;
            default: console::warn("unknown mouse button");
        }
    });

    onMouseScroll.connect([this](double offsetx, double offsety) {
        mouseScroll.x = offsetx;
        mouseScroll.y = offsety;
    });
}

InputManager::~InputManager() {
    onKeyPress.disconnect_all_slots();
    onMouseMove.disconnect_all_slots();
    onMousePress.disconnect_all_slots();
}

bool InputManager::isPress(KeyboardKey key) {
    return keyboardKeys_.test(key);
}

bool InputManager::isPress(KeyboardKey key1, KeyboardKey key2) {
    return isPress(key1) && isPress(key2);
}

bool InputManager::isPress(KeyboardModifier modifier, KeyboardKey key) {
    return keyboardModifiers_.test(modifier) && isPress(key);
}

bool InputManager::isPress(MouseButton button) {
    return mouseKeys_.test(button);
}

void InputManager::setMousePosition(ScreenCoord x, ScreenCoord y) {
    mouse.x = x;
    mouse.y = y;
}

void InputManager::setMouseStartPosition(ScreenCoord x, ScreenCoord y) {
    mouseStart.x = x;
    mouseStart.y = y;
}

void InputManager::setMouseMovedPosition(ScreenCoord x, ScreenCoord y) {
    mouseMoved.x = x;
    mouseMoved.y = y;
}

void InputManager::calcSensetivity(int width, int height, double dpi) {
    sensetivity.speedFactor = static_cast<float>(dpi) / 1000.0f;
    sensetivity.sensetivity = static_cast<float>(dpi) / 10000.0f;
}

void InputManager::update() {
    setMouseMovedPosition(mouseStart.x - mouse.x, mouseStart.y - mouse.y);
    setMouseStartPosition(mouse.x, mouse.y); // reset start position
}
