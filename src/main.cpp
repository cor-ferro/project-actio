#include <future>
#include <algorithm>

#include "main.h"

#include "lib/tweaker.h"

void renderHandler(ProgramContext& context) {
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
    const float elapsedTime = 16.6f;

    context.getEngine()->loadStory("story1", "chapter1");

    while (context.shouldWork) {
        auto start = std::chrono::system_clock::now();

//        bool hasTasks = world->hasTasks();
//
//        if (hasTasks) {
//            auto *task = world->popTaskToPerform();
//
//            if (task == nullptr) {
//                continue;
//            }
//
//            std::thread t([task, &workingSecondaryThreads, &world]() {
//                std::thread::id tid = std::this_thread::get_id();
//                console::info("make task thread: %i", tid);
//
//                workingSecondaryThreads.insert({tid, true});
//
//                world->performTask(task);
//
//                auto it = workingSecondaryThreads.find(tid);
//
//                if (it != workingSecondaryThreads.end()) {
//                    workingSecondaryThreads.erase(it);
//                    console::info("complete thread: %i", tid);
//                } else {
//                    console::warn("unable found the thread %i", tid);
//                }
//            });
//
//            t.detach();
//        }

//        world->flush();
        context.getEngine()->updateWorlds(elapsedTime);

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

    std::shared_ptr<game::Engine> engine = game::createEngine(app);

    engine->enablePhysicDebug();
    engine->createWorld();

    ProgramContext context(app, engine);

// @todo: очистка ресурсов происходит после удаления world, получаем segmentation fault
//    std::shared_ptr<Assets> worldAssets(new Assets());
//    worldAssets->loadDefaultResources();
//    world->importAssets(worldAssets);

    auto thread1 = std::thread(renderHandler, std::ref(context));
    auto thread2 = std::thread(appHandler, std::ref(context));

    thread1.join();
    thread2.join();

    console::info("stop application");

    return 0;
}
