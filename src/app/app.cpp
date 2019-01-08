#include "app.h"
#include <memory>

#include "../lib/assets_loader.h"

static std::shared_ptr<Assets> BaseAssets;

void App::init(int argc, char **argv) {
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

Path App::resourcePath() const {
    return createPath(m_path, RESOURCE_DIR);
}

Path App::resourcePath(const std::string& fromPath) const {
    return createPath(m_path, RESOURCE_DIR, fromPath);
}

const resources::File App::resource(const std::string& path) const {
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

const AppPaths &App::getPaths() const {
    return paths;
}

std::shared_ptr<Assets>& App::GetBaseAssets() {
    return BaseAssets;
}

void App::LoadBaseAssets(const AppPaths& appPaths) {
    console::info("load base assets");
    BaseAssets.reset(new Assets(appPaths));

    assets::Loader assetsLoader(appPaths.resources);

    const std::vector<std::pair<std::string, Path>> defaultTextures = {
            {"DefaultDiffuseTexture", "default-diffuse.png"},
            {"DefaultNormalTexture", "default-normal.jpg"},
            {"DefaultSpecularTexture", "default-specular.png"},
            {"DefaultHeightTexture", "default-height.png"}
    };

    for (const auto &it : defaultTextures) {
        const std::string &textureName = it.first;
        const Path &texturePath = it.second;

//        BaseAssets->add<assets::Image>(assetsLoader, texturePath);
    }
}

void App::UnloadBaseAssets() {
    BaseAssets.reset();
    console::info("unload base assets");
}

App* App::instance() {
    return &app;
}