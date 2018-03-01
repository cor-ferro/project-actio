#ifndef APP_MONITOR_H_
#define APP_MONITOR_H_

#include <GLFW/glfw3.h>

struct Monitor {
    Monitor();
    Monitor(GLFWmonitor * glfwMonitor);

    const double getDpi() const;
    const int getWidth() const;
    const int getHeight() const;
    const int getWidthMm() const;
    const int getHeightMm() const;
    const char * getName() const;

    private:
        double dpi = 0.0;
        int refreshRate = 0;
        int width = 0;
        int height = 0;
        int widthMm = 0;
        int heightMm = 0;
        const char * name;
};

#endif /* APP_H_ */
