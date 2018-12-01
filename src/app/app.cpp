#include "app.h"

App::App(int argc, char **argv) {
    console::warn("create app");
    m_name = "";

    argc_ = argc;
    argv_ = argv;

    m_path = ".";

    paths.resources = createPath(m_path, RESOURCE_DIR);
    paths.shaders = createPath(m_path, SHADERS_DIR);
}

const std::string App::getName() const {
    return m_name;
}

Path App::getPath() {
    return m_path;
}

Path App::resourcePath() {
    return createPath(m_path, RESOURCE_DIR);
}

Path App::resourcePath(const std::string& fromPath) {
    return createPath(m_path, RESOURCE_DIR, fromPath);
}

const resources::File App::resource(const std::string& path) {
    Path resourceFilePath = resourcePath(path);

    resources::File file(resourceFilePath.string());

    return file;
}

void App::setName(const std::string& name) {
    m_name = name;
}

void App::addMonitor(GLFWmonitor *monitor) {
    monitors.insert({(void *) monitor, Monitor(monitor)});
}

void App::removeMonitor(GLFWmonitor *monitor) {
    auto it = monitors.find((void *) monitor);

    if (it == monitors.end()) {
        console::warn("try remove non exists monitor");
        return;
    }

    monitors.erase(it);
}

const Monitor *const App::getPrimaryMonitor() {
    GLFWmonitor *ptr = glfwGetPrimaryMonitor();

    auto it = monitors.find((void *) ptr);

    if (it == monitors.end()) {
        return nullptr;
    }

    return &it->second;
}

void App::InitGLFW() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    glfwSetErrorCallback(w1ErrorCallback);

    int countMonitors = 0;
    GLFWmonitor **monitors = glfwGetMonitors(&countMonitors);

    for (int i = 0; i < countMonitors; i++) {
        addMonitor(monitors[i]);
    }

    glfwSetMonitorCallback([](GLFWmonitor *monitor, int event) {
        switch (event) {
            case GLFW_CONNECTED:
                console::info("monitor connected");
                // addMonitor(monitor);
                break;

            case GLFW_DISCONNECTED:
                console::info("monitor disconnected");
                // this->removeMonitor(monitor);
                break;
        }
    });
}

void App::TerminateGLFW() {
    glfwTerminate();
}

const AppPaths &App::getPaths() {
    return paths;
}
