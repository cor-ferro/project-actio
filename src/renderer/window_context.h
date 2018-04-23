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
    WindowContext();

    bool init(unsigned int width, unsigned int height);

    void setAsCurrent();

    void destroy();

    void setTitle(const char *text);

    void setSize(unsigned int width, unsigned int height);

    void setMousePosition(unsigned int x, unsigned int y);

    void getMousePosition(double &xpos, double &ypos);

    void enableVSync();

    void disableVSync();

    GLFWwindow *const getWindow();

    static void onResizeCallback(GLFWwindow *window, int width, int height);

    static void onKeyPressCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

    static void onMouseMoveCallback(GLFWwindow *window, double x, double y);

    static void onMousePressCallback(GLFWwindow *window, int button, int action, int mods);

    boost::signals2::signal<void(int, int)> onResize;
    boost::signals2::signal<void(int, int, int, int)> onKeyPress;
    boost::signals2::signal<void(double, double)> onMouseMove;
    boost::signals2::signal<void(int, int, int)> onMousePress;

private:
    GLFWwindow *window;
};

#endif
