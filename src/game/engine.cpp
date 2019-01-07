#include "engine.h"

#include "tasks/load_story.h"

namespace game {
    Engine::Engine(App& app) : m_app(app) {
        m_context.reset(new Context(io));
    }

    Engine::~Engine() {
        console::info("destroy engine");
    }

    void Engine::setupWorlds() {
        for (auto& world : m_worlds) {
            world.setup();
        }
    }

    void Engine::startWorlds() {
        for (auto& world : m_worlds) {
            world.start();
        }
    }

    void Engine::update() {
        m_tasksManager.process();

        if (m_tasksManager.hasCompleted()) {
            auto &completedTasks = m_tasksManager.getCompleted();

            while (!completedTasks.empty()) {
                Task *const task = completedTasks.front();
                completedTasks.pop();

                task->onFlush();

                delete task;
            }
        }
    }

    void Engine::updateWorlds(float elapsedTime) {
        for (auto& world : m_worlds) {
            world.update(elapsedTime);
        }
    }

    void Engine::destroyWorlds() {
        m_worlds.clear();
    }

    void Engine::render(const ex::TimeDelta& dt) {
        for (auto& world : m_worlds) {
            world.render(dt);
        }

        io.input->update();
    }

    void Engine::reset() {
        m_worlds.clear();
    }

    void Engine::enablePhysicDebug() {
//            m_physic->enableDebug();
    }

    void Engine::disablePhysicDebug() {
//            m_physic->disableDebug();
    }

    void Engine::initWindowContext(const Monitor *const monitor) {
        auto windowWidth = static_cast<renderer::Dimension>(monitor->getWidth() / 1.5);
        auto windowHeight = static_cast<renderer::Dimension>(monitor->getHeight() / 1.5);

        io.windowContext.reset(new WindowContext());

        if (!io.windowContext->init(windowWidth, windowHeight)) {
            throw std::runtime_error("failed initialize OpenGL context");
        }

        io.windowContext->setTitle(m_app.getName());
        io.windowContext->setAsCurrent();
        io.windowContext->enableVSync();

        io.input.reset(new InputManager());
        io.input->calcSensetivity(monitor->getWidth(), monitor->getHeight(), monitor->getDpi());

        io.windowContext->onKeyPress.connect(io.input->onKeyPress);
//    io.windowContext.onKeyPress.connect([&inputManager](int key, int scancode, int action, int mods) {
//        inputManager.onKeyPress(key, scancode, action, mods);
//    });

        io.windowContext->onMouseMove.connect(io.input->onMouseMove);
//    io.windowContext->onMouseMove.connect([&inputManager](double x, double y) {
//        inputManager.onMouseMove(x, y);
//    });

        io.windowContext->onMousePress.connect(io.input->onMousePress);
//    io.windowContext->onMousePress.connect([&inputManager](int button, int action, int mods) {
//        inputManager.onMousePress(button, action, mods);
//    });
    }

    void Engine::createRenderer() {
        destroyRenderer();

        auto size = io.windowContext->getSize();

        renderer::Params rendererParams;
        rendererParams.setWidth(size.width);
        rendererParams.setHeight(size.height);

        io.renderer.reset(renderer::create(rendererParams));
        io.renderer->setShadingType(renderer::Shading::Deferred);
        io.renderer->init();

        io.windowContext->onResize.connect([this](int width, int height) {
            console::info("resize: %i, %i", width, height);
//            renderer->setViewSize(static_cast<renderer::Dimension>(width), static_cast<renderer::Dimension>(height));
//            renderer->regenerateBuffer();
//                world->setRenderSize(width, height);
        });

        for (auto& world : m_worlds) {
            world.setRenderer(io.renderer);
        }
    }

    void Engine::destroyRenderer() {
        if (io.renderer) {
            io.renderer->destroy();
            io.renderer.reset();
        }
    }

    WindowContext& Engine::getWindowContext() {
        return *io.windowContext;
    }

    InputManager& Engine::getInput() {
        return *io.input;
    }

    World& Engine::createWorld() {
        m_worlds.emplace_back(*m_context.get());

        return m_worlds.back();
    }

    void Engine::startLoadStory(const std::string &storyName, const std::string &chapterName) {
        World& newWorld = createWorld();

        if (io.renderer) {
            newWorld.setRenderer(io.renderer);
        }

        auto *task = new TaskLoadStory(m_app, *m_context.get());
        task->setStoryName(storyName);
        task->setChapterName(chapterName);

        m_tasksManager.addNewTask(task);
    }

    App& Engine::app() const {
        return m_app;
    }
}