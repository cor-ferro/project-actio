#include "input_handler.h"

InputHandler::InputHandler(WindowContext &windowContext)
        : keyboardKeys_(0)
        , keyboardModifiers_(0)
        , mouseKeys_(0)
        , window(&windowContext) {
    windowContext.onKeyPress.connect([this](int key, int scancode, int action, int mods) {
        onKeyPress(key, scancode, action, mods);
    });

    windowContext.onMouseMove.connect([this](double x, double y) {
        onMouseMove(x, y);
    });

    windowContext.onMousePress.connect([this](int button, int action, int mods) {
        onMouseClick(button, action, mods);
    });

    double xpos, ypos;
    windowContext.getMousePosition(xpos, ypos);

    setMousePosition(static_cast<ScreenCoord>(xpos), static_cast<ScreenCoord>(ypos));
    setMouseStartPosition(static_cast<ScreenCoord>(xpos), static_cast<ScreenCoord>(ypos));
}

void InputHandler::onFrame() {
    setMouseMovedPosition(mouseStart.x - mouse.x, mouseStart.y - mouse.y);
    setMouseStartPosition(mouse.x, mouse.y); // reset start position
}

bool InputHandler::isPress(KeyboardKey key) {
    return keyboardKeys_.test(key);
}

bool InputHandler::isPress(KeyboardKey key1, KeyboardKey key2) {
    return isPress(key1) && isPress(key2);
}

bool InputHandler::isPress(KeyboardModifier modifier, KeyboardKey key) {
    return keyboardModifiers_.test(modifier) && isPress(key);
}

bool InputHandler::isPress(MouseButton button) {
    return mouseKeys_.test(button);
}

void InputHandler::setMousePosition(ScreenCoord x, ScreenCoord y) {
    mouse.x = x;
    mouse.y = y;
}

void InputHandler::setMouseStartPosition(ScreenCoord x, ScreenCoord y) {
    mouseStart.x = x;
    mouseStart.y = y;
}

void InputHandler::setMouseMovedPosition(ScreenCoord x, ScreenCoord y) {
    mouseMoved.x = x;
    mouseMoved.y = y;
}

void InputHandler::calcSensetivity(int width, int height, double dpi) {
    sensetivity.speedFactor = static_cast<float>(dpi) / 1000.0f;
    sensetivity.sensetivity = static_cast<float>(dpi) / 10000.0f;
}

void InputHandler::subscribeMouseMove(const std::function<void(double, double)>& f) {
    window->onMouseMove.connect(f);
}

void InputHandler::subscribeMousePress(const std::function<void(int, int, int)>& f) {
    window->onMousePress.connect(f);
}

void InputHandler::subscribeKeyPress(const std::function<void(int, int, int, int)>& f) {
    window->onKeyPress.connect(f);
}

void InputHandler::onKeyPress(int key, int scancode, int action, int mods) {
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
}

void InputHandler::onMouseMove(double x, double y) {
    setMousePosition(static_cast<ScreenCoord>(x), static_cast<ScreenCoord>(y));
}

void InputHandler::onMouseClick(int button, int action, int mods) {
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
}
