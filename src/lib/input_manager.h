#ifndef INPUT_HANDLER_H_
#define INPUT_HANDLER_H_

#include <bitset>
#include <iostream>
#include <algorithm>
#include <map>
#include <utility>
#include <boost/bind.hpp>
#include <boost/signals2.hpp>
#include "console.h"
#include "../renderer/window_context.h"

typedef float ScreenCoord;

struct MousePosition {
    MousePosition() : x(0), y(0) {}

    ScreenCoord x;
    ScreenCoord y;
};

struct InputManager {
    enum KeyboardKey {
        KEY_W = GLFW_KEY_W,
        KEY_A = GLFW_KEY_A,
        KEY_S = GLFW_KEY_S,
        KEY_D = GLFW_KEY_D,
        KEY_C = GLFW_KEY_C,
        KEY_E = GLFW_KEY_E,
        KEY_B = GLFW_KEY_B,
        KEY_F = GLFW_KEY_F,
        KEY_G = GLFW_KEY_G,
        KEY_H = GLFW_KEY_H,
        KEY_I = GLFW_KEY_I,
        KEY_J = GLFW_KEY_J,
        KEY_K = GLFW_KEY_K,
        KEY_L = GLFW_KEY_L,
        KEY_M = GLFW_KEY_M,
        KEY_N = GLFW_KEY_N,
        KEY_O = GLFW_KEY_O,
        KEY_P = GLFW_KEY_P,
        KEY_Q = GLFW_KEY_Q,
        KEY_R = GLFW_KEY_R,
        KEY_T = GLFW_KEY_T,
        KEY_U = GLFW_KEY_U,
        KEY_V = GLFW_KEY_V,
        KEY_X = GLFW_KEY_X,
        KEY_Y = GLFW_KEY_Y,
        KEY_Z = GLFW_KEY_Z,
        KEY_SPACE = GLFW_KEY_SPACE,
        KEY_ENTER = GLFW_KEY_ENTER,
        KEY_ESC = GLFW_KEY_ESCAPE,
        KEY_TAB = GLFW_KEY_TAB,
        KEY_BACKSPACE = GLFW_KEY_BACKSPACE,
        KEY_INSERT = GLFW_KEY_INSERT,
        KEY_DELETE = GLFW_KEY_DELETE,
        KEY_RIGHT = GLFW_KEY_RIGHT,
        KEY_LEFT = GLFW_KEY_LEFT,
        KEY_DOWN = GLFW_KEY_DOWN,
        KEY_UP = GLFW_KEY_UP,
        KEY_PAGE_UP = GLFW_KEY_PAGE_UP,
        KEY_PAGE_DOWN = GLFW_KEY_PAGE_DOWN,
        KEY_HOME = GLFW_KEY_HOME,
        KEY_END = GLFW_KEY_END,
        KEY_CAPS_LOCK = GLFW_KEY_CAPS_LOCK,
        KEY_SCROLL_LOCK = GLFW_KEY_SCROLL_LOCK,
        KEY_NUM_LOCK = GLFW_KEY_NUM_LOCK,
        KEY_PRINT_SCREEN = GLFW_KEY_PRINT_SCREEN,
        KEY_PAUSE = GLFW_KEY_PAUSE,
    };

    enum KeyboardModifier {
        MODIFIER_SHIFT = GLFW_MOD_SHIFT,
        MODIFIER_CONTROL = GLFW_MOD_CONTROL,
        MODIFIER_ALT = GLFW_MOD_ALT,
        MODIFIER_SUPER = GLFW_MOD_SUPER
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

    InputManager();

//    explicit InputManager(WindowContext &windowContext);

    ~InputManager();

    void update();

    bool isPress(KeyboardKey key);

    bool isPress(KeyboardKey key1, KeyboardKey key2);

    bool isPress(KeyboardModifier, KeyboardKey);

    bool isPress(MouseButton);

    void setMousePosition(ScreenCoord x, ScreenCoord y);

    void setMouseMovedPosition(ScreenCoord x, ScreenCoord y);

    void setMouseStartPosition(ScreenCoord x, ScreenCoord y);

    void calcSensetivity(int width, int height, double dpi);

    MousePosition mouse;
    MousePosition mouseStart;
    MousePosition mouseMoved;
    MousePosition mouseScrollStart;
    MousePosition mouseScroll;

    Sensetivity sensetivity;

    boost::signals2::signal<void(int, int, int, int)> onKeyPress;
    boost::signals2::signal<void(double, double)> onMouseMove;
    boost::signals2::signal<void(int, int, int)> onMousePress;
    boost::signals2::signal<void(double, double)> onMouseScroll;

private:
    std::bitset<GLFW_KEY_LAST> keyboardKeys_;
    std::bitset<GLFW_MOD_SUPER> keyboardModifiers_;
    std::bitset<GLFW_MOUSE_BUTTON_LAST> mouseKeys_;
};

#endif
