#ifndef WINDOW_CONTEXT_H
#define WINDOW_CONTEXT_H

#include <cstdint>
#include <boost/signals2.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include "../imgui_impl_glfw_gl3.h"
#include "../lib/console.h"


struct WindowContext {
    WindowContext()
            : window(nullptr) {
    }

    bool init(unsigned int width, unsigned int height) {
        window = glfwCreateWindow(width, height, "Window", nullptr, nullptr);

        if (window == nullptr) {
            console::info("Failed to create GLFW window");
            glfwTerminate();
            return false;
        }

        setAsCurrent();
        gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

        glfwSetWindowUserPointer(window, this);
//        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//        glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);

        ImGui_ImplGlfwGL3_Init(window, true);

        // add callbacks
        glfwSetWindowSizeCallback(window, WindowContext::onResizeCallback);
        glfwSetKeyCallback(window, WindowContext::onKeyPressCallback);
        glfwSetCursorPosCallback(window, WindowContext::onMouseMoveCallback);
        glfwSetMouseButtonCallback(window, WindowContext::onMousePressCallback);

        return true;
    }

    void setAsCurrent() {
        glfwMakeContextCurrent(window);
    }

    void destroy() {
        glfwDestroyWindow(window);
        window = nullptr;
    }

    void setTitle(const char *text) {
        glfwSetWindowTitle(window, text);
    }

    void setSize(unsigned int width, unsigned int height) {
        glfwSetWindowSize(window, width, height);
    }

    void setMousePosition(unsigned int x, unsigned int y) {
        glfwSetCursorPos(window, x, y);
    }

    void getMousePosition(double &xpos, double &ypos) {
        glfwGetCursorPos(window, &xpos, &ypos);
    }

    void enableVSync() {
        glfwSwapInterval(1);
    }

    void disableVSync() {
        glfwSwapInterval(0);
    }

    GLFWwindow *const getWindow() {
        return window;
    }

    static void onResizeCallback(GLFWwindow *window, int width, int height) {
        ((WindowContext *) glfwGetWindowUserPointer(window))->onResize(width, height);
    }

    static void onKeyPressCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
        ((WindowContext *) glfwGetWindowUserPointer(window))->onKeyPress(key, scancode, action, mods);
    }

    static void onMouseMoveCallback(GLFWwindow *window, double x, double y) {
        ((WindowContext *) glfwGetWindowUserPointer(window))->onMouseMove(x, y);
    }

    static void onMousePressCallback(GLFWwindow *window, int button, int action, int mods) {
        ((WindowContext *) glfwGetWindowUserPointer(window))->onMousePress(button, action, mods);
    }

    boost::signals2::signal<void(int, int)> onResize;
    boost::signals2::signal<void(int, int, int, int)> onKeyPress;
    boost::signals2::signal<void(double, double)> onMouseMove;
    boost::signals2::signal<void(int, int, int)> onMousePress;

private:
    GLFWwindow *window;
};

#endif
