#include <future>
#include <algorithm>
#include <mutex>
#include <condition_variable>

#include "main.h"

#include "lib/tweaker.h"
#include "lib/mesh_manager.h"

static std::mutex app_mtx;
static std::condition_variable app_condition;

void renderHandler(ProgramContext& context) {
    console::info("start render thread");
    context.getApp().InitGLFW();

    const Monitor *const monitor = context.getApp().getPrimaryMonitor();

    auto &engine = context.getEngine();

    engine->initWindowContext(monitor);
    engine->createRenderer();

    GLFWwindow *const window = engine->getWindowContext().getWindow();

    while (!glfwWindowShouldClose(window)) {
        #ifdef SAFE_THREADS
        std::unique_lock<std::mutex> lock(app_mtx);
        app_condition.wait(lock);
        #endif

        auto elapsedTime = static_cast<float>(glfwGetTime());

        engine->render(elapsedTime);

        glfwSwapBuffers(window);
        glfwPollEvents();

//        if (inputHandler->isPress(InputManager::KEY_ESC)) {
//            context.shouldWork = false;
//            break;
//        }
    }

    engine->destroyRenderer();
    engine->getWindowContext().destroy();

    context.getApp().TerminateGLFW();
}

void appHandler(ProgramContext& context) {
    console::info("start app thread");
    const float elapsedTime = 16.6f;

    auto engine = context.getEngine();

    auto &world = engine->createWorld();

    engine->setupWorlds();
    engine->startWorlds();
    engine->enableDebug();

    engine->startLoadStory(world, "story1", "chapter1");

    while (context.shouldWork) {
        #ifdef SAFE_THREADS
        std::unique_lock<std::mutex> lock(app_mtx);
        #endif

        auto start = std::chrono::system_clock::now();

        engine->update();
        engine->updateWorlds(elapsedTime);

        auto end = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        auto wait = 16600 - elapsed.count();
        if (wait < 0) {
            console::warn("too slow computations");
            wait = 0;
        }

        #ifdef SAFE_THREADS
        lock.unlock();
        app_condition.notify_all();
        #endif

        std::this_thread::sleep_for(std::chrono::microseconds(wait));
    }

    context.getEngine()->destroyWorlds();
}

int main(int argc, char **argv) {
    App::instance()->init(argc, argv);
    App::instance()->setName("project actio");

    console::info("start %s", App::instance()->getName());

//    App::LoadBaseAssets(app.getPaths());

    std::shared_ptr<game::Engine> engine = game::createEngine(app);

    engine->enableDebug();
    engine->createWorld();

    ProgramContext context(app, engine);

    auto thread1 = std::thread(renderHandler, std::ref(context));
    auto thread2 = std::thread(appHandler, std::ref(context));

    thread1.join();
    thread2.join();

//    App::UnloadBaseAssets();

    console::info("stop application");

    return 0;
}
