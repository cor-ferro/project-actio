#include <future>
#include "main.h"
#include "renderer/init.h"
//#include "renderer_old/renderer_types.h"
#include "renderer/renderer.h"
//#include "renderer_old/shader_description.h"
#include "game/world_importer.h"
#include "lib/tweaker.h"
#include "game/story_importer.h"

int main(int argc, char **argv) {
    App &app = App::instance();
    app.setName("project actio");
    app.init(argc, argv);

    console::info("start %s", app.getName());

    std::string storyName = "story1";
    std::string chapterName = "chapter1";

    const Resource::File storyFile = app.resource(storyName + "/story.yaml");

    game::StoryImporter storyImporter;
    game::Story story = storyImporter.import(storyFile);

    const size_t maxModels = 10000;
    const size_t maxMeshes = 50000;

    imageAllocator = new memory::FreeListAllocator(1e8, memory::FreeListAllocator::FIND_FIRST);
    imageAllocator->Init();

    modelsAllocator = new memory::PoolAllocator(sizeof(Model) * maxModels, sizeof(Model));
    modelsAllocator->Init();

    meshAllocator = new memory::PoolAllocator(sizeof(Mesh) * maxMeshes, sizeof(Mesh));
    meshAllocator->Init();

    const Monitor *const monitor = app.getPrimaryMonitor();

    renderer::Dimension windowWidth = monitor->getWidth() / 1.5;
    renderer::Dimension windowHeight = monitor->getHeight() / 1.5;

    printMemoryStatus();

    const Resource::File testWorldFile = app.resource("testScene.ini");

    WorldSettings guiSettings;


    game::World *world = game::createWorld();
    world->setup();
    world->setLightDebug(guiSettings.debugLight.getRef());

    // @todo: очистка ресурсов происходит после удаления world, получаем segmentation fault
    std::shared_ptr<Assets> worldAssets(new Assets());
    worldAssets->loadDefaultResources();
    world->importAssets(worldAssets);

    auto worldImporter = new game::WorldImporter(world);
    worldImporter->import(testWorldFile);

    bool statusChapterLoad = storyImporter.loadChapterAssets(story, chapterName, worldAssets.get());

    if (!statusChapterLoad) {
        console::err("failed load chapter: %s", chapterName);

        return -1;
    }

    world->load();
    world->generateBaseTerrain();

    bool renderExec = true;

    auto renderThread = std::thread([&]() {
        WindowContext mainContext;

        if (!mainContext.init(windowWidth, windowHeight)) {
            return false;
        }

        mainContext.setTitle(app.getName().c_str());
        mainContext.setAsCurrent();
        mainContext.enableVSync();

        renderer::Params rendererParams;
        rendererParams.setWidth(windowWidth);
        rendererParams.setHeight(windowHeight);

        renderer::Renderer *renderer = renderer::create(rendererParams);
        renderer->setShadingType(renderer::Shading::Deferred);
        renderer->init();

        world->setRenderer(renderer);

        mainContext.onResize.connect([&renderer, &world](int width, int height) {
            console::info("resize: %i, %i", width, height);
//            renderer->setViewSize(static_cast<renderer::Dimension>(width), static_cast<renderer::Dimension>(height));
//            renderer->regenerateBuffer();

            world->setRenderSize(width, height);
        });

        auto *inputHandler = new InputHandler(mainContext);
        inputHandler->calcSensetivity(monitor->getWidth(), monitor->getHeight(), monitor->getDpi());

        world->setInput(1, inputHandler);

        GLFWwindow *const window = mainContext.getWindow();

        while (!glfwWindowShouldClose(window)) {
            auto elapsedTime = static_cast<float>(glfwGetTime());

            world->render(elapsedTime);

//            const renderer::Stats& renderStats = renderer->getStats();
            {
                if (guiSettings.debugLight.isChanged(true)) world->setLightDebug(guiSettings.debugLight.getRef());
                if (guiSettings.debugPhysics.isChanged(true)) world->setPhysicsDebug(guiSettings.debugPhysics.getRef());
                if (guiSettings.cameraFov.isChanged(true) ||
                    guiSettings.cameraAspect.isChanged(true) ||
                    guiSettings.cameraNear.isChanged(true) ||
                    guiSettings.cameraFar.isChanged(true)) {
                    world->setCameraSettings(
                            guiSettings.cameraFov.getRef(),
                            guiSettings.cameraAspect.getRef(),
                            guiSettings.cameraNear.getRef(),
                            guiSettings.cameraFar.getRef()
                    );
                }

                ImGui_ImplGlfwGL3_NewFrame();
                ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), true);

                ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar
                                         | ImGuiWindowFlags_NoMove
                                         | ImGuiWindowFlags_NoResize
                                         | ImGuiWindowFlags_NoBringToFrontOnFocus;

                ImGui::Begin("Metrics", nullptr, flags);
//                ImGui::Text("%.3f ms, %.1f fps", renderStats.getMsFrame(), renderStats.getFps());
//                ImGui::Text("draw calls: %i", renderStats.getDrawCalls());
                ImGui::Text("images: %s", utils::formatMemorySize(imageAllocator->getUsed()).c_str());
                ImGui::Text("models: %s", utils::formatMemorySize(modelsAllocator->getUsed()).c_str());
                ImGui::Text("meshes: %s", utils::formatMemorySize(meshAllocator->getUsed()).c_str());
                ImGui::Separator();
                ImGui::Checkbox("Debug physics", guiSettings.debugPhysics.get());
                ImGui::Checkbox("Debug light", guiSettings.debugLight.get());
                ImGui::Separator();
                ImGui::SliderFloat("fow", guiSettings.cameraFov.get(), 0.0f, 180.0f, "fow = %.3f");
                ImGui::SliderFloat("aspect", guiSettings.cameraAspect.get(), 0.0f, 5.0f, "aspect = %.3f");
                ImGui::SliderFloat("near", guiSettings.cameraNear.get(), 0.0f, 500.0f, "near = %.3f");
                ImGui::SliderFloat("far", guiSettings.cameraFar.get(), 0.0f, 500.0f, "far = %.3f");

                const profiling::ProfileTimings &worldSystemProfiler = world->getSystemProfiler();
                const profiling::TimePrecision physicDuration = worldSystemProfiler.getDuration("Physic", profiling::Milliseconds);
                const profiling::TimePrecision renderDuration = worldSystemProfiler.getDuration("Render", profiling::Milliseconds);
                const profiling::TimePrecision worldUpdateDuration = worldSystemProfiler.getDuration("WorldUpdate", profiling::Milliseconds);

                ImGui::Separator();
                ImGui::Text("world: %.2f", worldUpdateDuration);
                ImGui::Text("physic: %.2f", physicDuration);
                ImGui::Text("render: %.2f", renderDuration);

                ImGui::SetWindowSize(ImVec2(200.0f, 350.0f));
                ImGui::End();
                ImGui::Render();
            }

            inputHandler->onFrame();

            glfwSwapBuffers(window);
            glfwPollEvents();

            if (inputHandler->isPress(InputHandler::KEY_ESC)) {
                world->destroyRenderer();
                mainContext.destroy();
                delete renderer;
                break;
            }
        }

        renderExec = false;
    });

//    std::mutex m;
//    std::condition_variable cv;
//
//    auto appSecondThread = std::thread([&]() {
//        std::unique_lock<std::mutex> lk(m);
//
//
//    });
//
    std::map<std::thread::id, bool> workingSecondaryThreads;

    auto appThread = std::thread([&]() {
        const float elapsedTime = 16.6f;

        while (renderExec) {
            bool hasTasks = world->hasTasks();

            if (hasTasks) {
                auto *task = world->popTaskToPerform();

                if (task == nullptr) {
                    continue;
                }

                std::thread t([task, &workingSecondaryThreads, &world]() {
                    std::thread::id tid = std::this_thread::get_id();
                    console::info("make task thread: %i", tid);

                    workingSecondaryThreads.insert({tid, true});

                    world->performTask(task);

                    auto it = workingSecondaryThreads.find(tid);

                    if (it != workingSecondaryThreads.end()) {
                        workingSecondaryThreads.erase(it);
                        console::info("complete thread: %i", tid);
                    } else {
                        console::warn("unable found the thread %i", tid);
                    }
                });

                t.detach();
            }

            world->flush();
            world->update(elapsedTime);

            std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>(1000.0f * elapsedTime)));
        }
    });

    // wait all loops finish
    renderThread.join();
//    appSecondThread.join();
    appThread.join();

    while (!workingSecondaryThreads.empty()) {
        console::info("wait all secondary threads finish");
    }

    world->destroy();
    delete world;

    // cleanupScene(scene);
    printMemoryStatus();

    delete imageAllocator;
    delete modelsAllocator;
    delete meshAllocator;

    console::info("stop application");

    return 0;
}

// void cleanupScene(Scene * scene)
// {
// 	std::vector<Model*> sceneModels = scene->getModels();

// 	for (uint i = 0; i < sceneModels.size(); i++) {
// 		Model * model = sceneModels[i];
// 		Model::Destroy(model);
// 	}
// }

void printMemoryStatus() {
    console::info("-------memory-------");
    console::info("images: %s", utils::formatMemorySize(imageAllocator->getUsed()));
    console::info("models: %s", utils::formatMemorySize(modelsAllocator->getUsed()));
    console::info("mesh:   %s", utils::formatMemorySize(meshAllocator->getUsed()));
    console::info("--------------------");
}