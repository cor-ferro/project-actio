#include "monitor.h"

Monitor::Monitor() {};

Monitor::Monitor(GLFWmonitor * monitor) {
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    int widthMM, heightMM;
    glfwGetMonitorPhysicalSize(monitor, &widthMM, &heightMM);

    widthMm = widthMM;
    heightMm = heightMM;
    width = mode->width;
    height = mode->height;
    dpi = mode->width / (widthMm / 25.4);
    name = glfwGetMonitorName(monitor);
}

const double Monitor::getDpi() const
{
    return dpi;
}

const int Monitor::getWidth() const
{
    return width;
}

const int Monitor::getHeight() const
{
    return height;
}

const int Monitor::getWidthMm() const
{
    return widthMm;
}

const int Monitor::getHeightMm() const
{
    return heightMm;
}

const char * Monitor::getName() const
{
    return name;
}
