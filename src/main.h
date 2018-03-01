#define NDEBUG

#include <thread>
#include <mutex>
#include <functional>
#include <chrono>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// #include <memory>
#include <signal.h>
#include <boost/bind.hpp>

#include "PxPhysics.h"
#include "PxScene.h"
#include "PxRigidDynamic.h"
#include "PxShape.h"
#include "PxPhysicsAPI.h"
#include "pvd/PxPvd.h"

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
#include "renderer/renderer.h"
#include "scenes/scene.h"
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

using namespace std;
using namespace physx;

int main(int argc, char **argv);

PxRigidDynamic* createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity=PxVec3(0));

void initPhysics();
void stepPhysics();
void cleanupPhysics();
void cleanupScene(Scene * scene);

void printMemoryStatus();

PxDefaultAllocator gAllocator;
PxDefaultErrorCallback gErrorCallback;

PxFoundation* gFoundation = NULL;
PxPhysics*  gPhysics = NULL;
PxPvd* gPvd = NULL;
PxDefaultCpuDispatcher* gDispatcher = NULL;
PxScene* gScene = NULL;
PxMaterial* gMaterial = NULL;

memory::FreeListAllocator * imageAllocator = nullptr;
memory::PoolAllocator * modelsAllocator = nullptr;
memory::PoolAllocator * meshAllocator = nullptr;