#include <thread>
#include <mutex>
#include <functional>
#include <chrono>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// #include <memory>
#include <signal.h>
#include <boost/bind.hpp>

#include "glm/gtc/random.hpp"

#include "app/app.h"
#include "ag.h"
#include "imgui_impl_glfw_gl3.h"

#include "renderer/window_context.h"
#include "lib/types.h"
#include "lib/cycle.h"
#include "lib/input_handler.h"
#include "lib/console.h"
#include "lib/utils.h"
#include "cameras/camera.h"
#include "cameras/camera_control.h"
#include "lights/direction_light.h"
#include "core/model.h"
#include "helpers/arrow_helper.h"
#include "helpers/bounding_box_helper.h"
#include "helpers/point_light_helper.h"
#include "helpers/camera_orientation_helper.h"
#include "resources/resources.h"
#include "memory/freelistallocator.h"
#include "memory/linearallocator.h"
#include "memory/poolallocator.h"

#include "game/game.h"

using namespace std;

int main(int argc, char **argv);

// void cleanupScene(Scene * scene);

void printMemoryStatus();

struct WorldSettings {
    WorldSettings() {}

    WorldSettings(const WorldSettings &other) {
        debugPhysics = other.debugPhysics;
        debugLight = other.debugLight;
        cameraFov = other.cameraFov;
        cameraAspect = other.cameraAspect;
        cameraNear = other.cameraNear;
        cameraFar = other.cameraFar;
    }

    bool debugPhysics;
    bool debugLight;
    float cameraFov = 45.0f;
    float cameraAspect = 1.77f;
    float cameraNear = 0.1f;
    float cameraFar = 500.0f;
};

memory::FreeListAllocator *imageAllocator = nullptr;
memory::PoolAllocator *modelsAllocator = nullptr;
memory::PoolAllocator *meshAllocator = nullptr;