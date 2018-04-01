#include "input_handler.h"

InputHandler::InputHandler(WindowContext &windowContext)
        : keyboardModifiers_(0)
        , mouseModifiers_(0)
        , window(&windowContext) {
    windowContext.onKeyPress.connect([this](int key, int scancode, int action, int mods) {
        onKeyPress(key, scancode, action, mods);
    });

    windowContext.onMouseMove.connect([this](double x, double y) {
        onMouseMove(x, y);
    });

//    windowContext.onMousePress.connect([this](int button, int action, int mods) {});

    double xpos, ypos;
    windowContext.getMousePosition(xpos, ypos);

    setMousePosition(static_cast<ScreenCoord>(xpos), static_cast<ScreenCoord>(ypos));
    setMouseStartPosition(static_cast<ScreenCoord>(xpos), static_cast<ScreenCoord>(ypos));
}

void InputHandler::onFrame() {
    setMouseMovedPosition(mouseStart.x - mouse.x, mouseStart.y - mouse.y);
    setMouseStartPosition(mouse.x, mouse.y); // reset start position
}

bool InputHandler::isPress(KeyCode key) {
    return isKeyPress(key);
}

bool InputHandler::isPress(KeyCode key1, KeyCode key2) {
    return isKeyPress(key1) && isKeyPress(key2);
}

bool InputHandler::isPress(KeyboardModifier modifier, KeyCode key) {
    return isModifierPress(modifier) && isKeyPress(key);
}

bool InputHandler::isPress(MouseModifier modifier) {
    return isModifierPress(modifier);
}

bool InputHandler::isKeyPress(KeyCode key) {
    std::map<KeyCode, bool>::const_iterator it = map_.find(key);

    return it != map_.end() && it->second;
}

bool InputHandler::isModifierPress(KeyboardModifier modifier) {
    return keyboardModifiers_.test(modifier);
}

bool InputHandler::isModifierPress(MouseModifier modifier) {
    return mouseModifiers_.test(modifier);
}

void InputHandler::setKeyDown(KeyCode key) {
    auto it = map_.find(key);

    if (it != map_.end()) {
        it->second = true;
    } else {
        map_.insert({key, true});
    }
}

void InputHandler::setKeyUp(KeyCode key) {
    auto it = map_.find(key);
    if (it != map_.end()) {
        it->second = false;
    }
}

void InputHandler::setModifierDown(KeyboardModifier modifier) {
    keyboardModifiers_.set(modifier, true);
}

void InputHandler::setModifierUp(KeyboardModifier modifier) {
    keyboardModifiers_.set(modifier, false);
}

void InputHandler::setModifierDown(MouseModifier modifier) {
    mouseModifiers_.set(modifier, true);
}

void InputHandler::setModifierUp(MouseModifier modifier) {
    mouseModifiers_.set(modifier, false);
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
    switch (action) {
        case KEY_PRESS:
            setKeyDown(key);
            break;
        case KEY_RELEASE:
            setKeyUp(key);
            break;
    }
}

void InputHandler::onMouseMove(double x, double y) {
    setMousePosition(static_cast<ScreenCoord>(x), static_cast<ScreenCoord>(y));
}

void InputHandler::onMouseClick(int button, int state, int x, int y) {
    MouseModifier mouseModifier;

    switch (button) {
        case MOUSE_BUTTON_LEFT:
            mouseModifier = MouseLeft;
            break;
        case MOUSE_BUTTON_MIDDLE:
            mouseModifier = MouseMiddle;
            break;
        case MOUSE_BUTTON_RIGHT:
            mouseModifier = MouseRight;
            break;
    }

    switch (state) {
        case KEY_PRESS:
            setModifierDown(mouseModifier);
            break;
        case KEY_RELEASE:
            setModifierUp(mouseModifier);
            break;
    }
}
