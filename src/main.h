#include <thread>
#include <mutex>
#include <functional>
#include <chrono>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <boost/filesystem.hpp>
#include "lib/console.h"
#include "lib/path.h"

// #include <memory>
#include <signal.h>
#include <boost/bind.hpp>
#include <map>
#include <vector>
#include <list>
#include <string>

#include "app/app.h"
#include "imgui_impl_glfw_gl3.h"

#include "renderer/window_context.h"
#include "renderer/shader_content.h"
#include "renderer/shader_description.h"
#include "lib/types.h"
#include "lib/cycle.h"
#include "lib/input_handler.h"
#include "lib/console.h"
#include "lib/utils.h"
#include "lib/value_handler.h"
#include "cameras/camera.h"
#include "cameras/camera_control.h"
#include "lights/direction_light.h"
#include "core/model.h"
#include "resources/resources.h"
#include "memory/freelistallocator.h"
#include "memory/linearallocator.h"
#include "memory/poolallocator.h"

#include "game/game.h"

using namespace std;
namespace fs = boost::filesystem;

int main(int argc, char **argv);

// void cleanupScene(Scene * scene);

void printMemoryStatus();

struct WorldSettings {
    WorldSettings() {
        debugPhysics = true;
        debugLight = true;
        cameraFov = 45.0f;
        cameraAspect = 1.77f;
        cameraNear = 0.1f;
        cameraFar = 500.0f;
    }

    WorldSettings(const WorldSettings &other) {
        debugPhysics = other.debugPhysics;
        debugLight = other.debugLight;
        cameraFov = other.cameraFov;
        cameraAspect = other.cameraAspect;
        cameraNear = other.cameraNear;
        cameraFar = other.cameraFar;
    }

    ValueHandler<bool> debugPhysics;
    ValueHandler<bool> debugLight;
    ValueHandler<float> cameraFov;
    ValueHandler<float> cameraAspect;
    ValueHandler<float> cameraNear;
    ValueHandler<float> cameraFar;
};

void loadRequiredShaders(std::vector<renderer::ShaderDescription> *list) {
    Path folder("resources/required-shaders");

    fs::directory_iterator itr_end;
    std::map<std::string, std::vector<std::string>> shaders;

//    shaders.insert({ "edge", {"frag", "vert"} });
//    shaders.insert({ "forward", {"frag", "geom1", "vert"} });
//    shaders.insert({ "geometry_pass", {"frag", "vert"} });
//    shaders.insert({ "light_pass", {"frag", "vert"} });
//    shaders.insert({ "null", {"frag", "vert"} });
//    shaders.insert({ "skybox", {"frag", "vert"} });
//    shaders.insert({ "skybox-deferred", {"frag", "vert"} });

    for (fs::directory_iterator itr(folder); itr != itr_end; ++itr) {
        if (fs::is_regular_file(itr->path())) {
            std::string shaderName = itr->path().stem().string();
            std::string shaderType = itr->path().extension().string();

            shaderType.erase(0, 1);

            if (shaders.find(shaderName) == shaders.end()) {
                shaders[shaderName] = { shaderType };
            } else {
                shaders[shaderName].push_back(shaderType);
            }
        }
    }

    for (const auto &it : shaders) {
        const std::string name = it.first;
        const std::vector<std::string> &types = it.second;

        renderer::ShaderDescription shaderDescription;
        shaderDescription.setName(name);

        for (const auto &type : types) {
            Path shaderFilePath = createPath(folder, name);
            shaderFilePath.replace_extension(type.c_str());

            renderer::ShaderContent shaderContent;
            std::shared_ptr<std::string> &content = shaderContent.getContent();

            size_t s = 0;
            utils::getFileContents(shaderFilePath, content, &s);

            shaderDescription.setByType(type, shaderContent);
        }

        list->push_back(shaderDescription);
    }
}

memory::FreeListAllocator *imageAllocator = nullptr;
memory::PoolAllocator *modelsAllocator = nullptr;
memory::PoolAllocator *meshAllocator = nullptr;