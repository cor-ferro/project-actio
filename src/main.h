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
//#include "renderer_old/shader_content.h"
//#include "renderer_old/shader_description.h"
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

memory::FreeListAllocator *imageAllocator = nullptr;
memory::PoolAllocator *modelsAllocator = nullptr;
memory::PoolAllocator *meshAllocator = nullptr;