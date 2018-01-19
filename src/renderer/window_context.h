#ifndef WINDOW_CONTEXT_H
#define WINDOW_CONTEXT_H

#include <cstdint>
#include <boost/signals2.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
// #include "../imgui_impl_glfw_gl3.h"
#include "../lib/console.h"

static void w1ErrorCallback(int error, const char* description)
{
    console::err("GLFW error: ", description);
}

struct WindowContext {
    WindowContext()
        : window(nullptr)
    {
    }

    bool init(unsigned int width, unsigned int height) {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, 4);

        glfwSetErrorCallback(w1ErrorCallback);

        // GLFWmonitor* primary = glfwGetPrimaryMonitor();
        window = glfwCreateWindow(width, height, "Window", NULL, NULL);

        if (window == NULL)
        {
            console::info("Failed to create GLFW window");
            glfwTerminate();
            return false;
        }

        glfwMakeContextCurrent(window);
        gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

        glfwSetWindowUserPointer(window, this);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        // glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);

        // ImGui_ImplGlfwGL3_Init(window, true);

        // add callbacks
        glfwSetWindowSizeCallback(window, WindowContext::onResizeCallback);
        glfwSetKeyCallback(window, WindowContext::onKeyPressCallback);
	    glfwSetCursorPosCallback(window, WindowContext::onMouseMoveCallback);

        return true;
    }

    void destroy() {
        glfwDestroyWindow(window);
        window = nullptr;
    }

    void setTitle(const char * text) {
        glfwSetWindowTitle(window, text);
    }

    void setSize(unsigned int width, unsigned int height) {
        glfwSetWindowSize(window, width, height);
    }

    void setMousePosition(unsigned int x, unsigned int y) {
        glfwSetCursorPos(window, x, y);
    }

    void enableVSync() {
        glfwSwapInterval(1);
    }

    void disableVSync() {
        glfwSwapInterval(0);
    }

    GLFWwindow * const getWindow() {
        return window;
    }

    static void onResizeCallback(GLFWwindow * window, int width, int height) {
        ((WindowContext*)glfwGetWindowUserPointer(window))->onResize(width, height);
    }

    static void onKeyPressCallback(GLFWwindow * window, int key, int scancode, int action, int mods) {
        ((WindowContext*)glfwGetWindowUserPointer(window))->onKeyPress(key, scancode, action, mods);
    }

    static void onMouseMoveCallback(GLFWwindow * window, double x, double y) {
        ((WindowContext*)glfwGetWindowUserPointer(window))->onMouseMove(x, y);
    }

    boost::signals2::signal<void (int, int)> onResize;
    boost::signals2::signal<void (int, int, int, int)> onKeyPress;
    boost::signals2::signal<void (double, double)> onMouseMove;

    private:
        GLFWwindow * window;
};

#endif
