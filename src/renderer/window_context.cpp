#include "window_context.h"

WindowContext::WindowContext() : window(nullptr) {}

bool WindowContext::init(int width, int height) {
    window = glfwCreateWindow(width, height, "Window", nullptr, nullptr);

    if (window == nullptr) {
        console::info("Failed to create GLFW window");
        return false;
    }

    setAsCurrent();
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    glfwSetWindowUserPointer(window, this);
//    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//    glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);

    ImGui_ImplGlfwGL3_Init(window, true);

    // add callbacks
    glfwSetWindowSizeCallback(window, WindowContext::onResizeCallback);
    glfwSetKeyCallback(window, WindowContext::onKeyPressCallback);
    glfwSetCursorPosCallback(window, WindowContext::onMouseMoveCallback);
    glfwSetMouseButtonCallback(window, WindowContext::onMousePressCallback);

    return true;
}

void WindowContext::setAsCurrent() {
    glfwMakeContextCurrent(window);
}

void WindowContext::destroy() {
    glfwDestroyWindow(window);
    window = nullptr;
}

void WindowContext::setTitle(const std::string &title) {
    glfwSetWindowTitle(window, title.c_str());
}

void WindowContext::setSize(int width, int height) {
    glfwSetWindowSize(window, width, height);
}

void WindowContext::setMousePosition(unsigned int x, unsigned int y) {
    glfwSetCursorPos(window, x, y);
}

void WindowContext::getMousePosition(double &xpos, double &ypos) {
    glfwGetCursorPos(window, &xpos, &ypos);
}

void WindowContext::enableVSync() {
    glfwSwapInterval(1);
}

void WindowContext::disableVSync() {
    glfwSwapInterval(0);
}

GLFWwindow *const WindowContext::getWindow() {
    return window;
}

void WindowContext::onResizeCallback(GLFWwindow *window, int width, int height) {
        ((WindowContext *) glfwGetWindowUserPointer(window))->onResize(width, height);
    }

void WindowContext::onKeyPressCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
        ((WindowContext *) glfwGetWindowUserPointer(window))->onKeyPress(key, scancode, action, mods);
    }

void WindowContext::onMouseMoveCallback(GLFWwindow *window, double x, double y) {
        ((WindowContext *) glfwGetWindowUserPointer(window))->onMouseMove(x, y);
    }

void WindowContext::onMousePressCallback(GLFWwindow *window, int button, int action, int mods) {
        ((WindowContext *) glfwGetWindowUserPointer(window))->onMousePress(button, action, mods);
    }

WindowContext::Size WindowContext::getSize() const {
    assert(window != nullptr);

    WindowContext::Size size;
    glfwGetWindowSize(window, &size.width, &size.height);

    return size;
}
