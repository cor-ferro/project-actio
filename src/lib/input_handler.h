#ifndef INPUT_HANDLER_H_
#define INPUT_HANDLER_H_

#include <bitset>
#include <iostream>
#include <algorithm>
#include <map>
#include <utility>
#include <boost/bind.hpp>
#include "console.h"
#include "../renderer/window_context.h"

enum KeyboardModifier {
    KeyboardShift = 1,
    KeyboardAlt = 2,
    KeyboardCtrl = 4,
    KeyboardMeta = 8,
};

enum MouseModifier {
    MouseLeft = 1,
    MouseMiddle = 2,
    MouseRight = 4,
};

typedef float ScreenCoord;

struct MousePosition {
    MousePosition() : x(0), y(0) {}

    ScreenCoord x;
    ScreenCoord y;
};

typedef short int KeyCode;

struct InputHandler {
    enum KeyType {
        KEY_SPACE = GLFW_KEY_SPACE,
        KEY_ENTER = GLFW_KEY_ENTER,
        KEY_W = GLFW_KEY_W,
        KEY_A = GLFW_KEY_A,
        KEY_S = GLFW_KEY_S,
        KEY_D = GLFW_KEY_D,
        KEY_C = GLFW_KEY_C,
        KEY_ESC = GLFW_KEY_ESCAPE
    };

    enum KeyAction {
        KEY_PRESS = GLFW_PRESS,
        KEY_RELEASE = GLFW_RELEASE,
        KEY_REPEAT = GLFW_REPEAT
    };

    enum MouseButton {
        MOUSE_BUTTON_LEFT = GLFW_MOUSE_BUTTON_LEFT,
        MOUSE_BUTTON_MIDDLE = GLFW_MOUSE_BUTTON_MIDDLE,
        MOUSE_BUTTON_RIGHT = GLFW_MOUSE_BUTTON_RIGHT
    };

    struct Sensetivity {
        float sensetivity = 1.0f;
        float speedFactor = 1.0f;
        float speed = 1.0f;
    };

    explicit InputHandler(WindowContext &windowContext);

    bool isPress(KeyCode);

    bool isPress(KeyCode, KeyCode);

    bool isPress(KeyboardModifier, KeyCode);

    bool isPress(MouseModifier);

    void setKeyDown(KeyCode key);

    void setKeyUp(KeyCode key);

    void setModifierDown(KeyboardModifier modifier);

    void setModifierUp(KeyboardModifier modifier);

    void setMousePosition(ScreenCoord x, ScreenCoord y);

    void setMouseMovedPosition(ScreenCoord x, ScreenCoord y);

    void setMouseStartPosition(ScreenCoord x, ScreenCoord y);

    void setModifierDown(MouseModifier modifier);

    void setModifierUp(MouseModifier modifier);

    void onKeyPress(int key, int scancode, int action, int mods);

    void onMouseMove(double x, double y);

    void onMouseClick(int button, int state, int x, int y);

    void onFrame();

    void calcSensetivity(int width, int height, double dpi);

    void subscribeMouseMove(const std::function<void(double, double)>& f);

    void subscribeMousePress(const std::function<void(int, int, int)>& f);

    void subscribeKeyPress(const std::function<void(int, int, int, int)>& f);

    MousePosition mouse;
    MousePosition mouseStart;
    MousePosition mouseMoved;

    Sensetivity sensetivity;

private:
    bool isKeyPress(KeyCode);

    bool isModifierPress(KeyboardModifier);

    bool isModifierPress(MouseModifier);

    std::bitset<8> keyboardModifiers_;
    std::bitset<8> mouseModifiers_;
    std::map<KeyCode, bool> map_;

    WindowContext* window;
};

#endif
