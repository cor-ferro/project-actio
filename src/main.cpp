#include <future>
#include <algorithm>

#include "main.h"

#include "lib/tweaker.h"
#include "lib/mesh_manager.h"

void renderHandler(ProgramContext& context) {
    console::info("start render thread");
    context.getApp().InitGLFW();

    const Monitor *const monitor = context.getApp().getPrimaryMonitor();

    auto &engine = context.getEngine();

    engine->initWindowContext(monitor);
    engine->createRenderer();

    GLFWwindow *const window = engine->getWindowContext().getWindow();

    while (!glfwWindowShouldClose(window)) {
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

    engine->startLoadStory("story1", "chapter1");
    engine->setupWorlds();
    engine->startWorlds();

    while (context.shouldWork) {
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

        std::this_thread::sleep_for(std::chrono::microseconds(wait));
    }

    context.getEngine()->destroyWorlds();
}

int main(int argc, char **argv) {
    App app(argc, argv);
    app.setName("project actio");

    console::info("start %s", app.getName());

//    App::LoadBaseAssets(app.getPaths());

    std::shared_ptr<game::Engine> engine = game::createEngine(app);

    engine->enablePhysicDebug();
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
