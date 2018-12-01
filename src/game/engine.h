#ifndef ACTIO_ENGINE_H
#define ACTIO_ENGINE_H

#include <memory>
#include <list>
#include <string>
#include <thread>
#include <stdexcept>

#include "input_output.h"
#include "../renderer/init.h"
#include "../app/app.h"
#include "world.h"
#include "context.h"
#include "story_importer.h"
#include "../lib/input_manager.h"

namespace game {
    namespace ex = entityx;

    class Engine {
    public:
        explicit Engine(App& app) : m_app(app) {}

        void setupWorlds() {
            for (auto& world : m_worlds) {
                world.setup();
            }
        }

        void startWorlds() {
            for (auto& world : m_worlds) {
                world.start();
            }
        }

        void updateWorlds(float elapsedTime) {
            for (auto& world : m_worlds) {
                world.update(elapsedTime);
            }
        }

        void destroyWorlds() {
            m_worlds.clear();
        }

        void render(const ex::TimeDelta& dt) {
            for (auto& world : m_worlds) {
                world.render(dt);
            }

            io.input->onFrameUpdate();
        }

        void reset() {
            m_worlds.clear();
        }

        void setInput(InputManager *inputManager) {
//            context->
        }

        void enablePhysicDebug() {
//            m_physic->enableDebug();
        }

        void disablePhysicDebug() {
//            m_physic->disableDebug();
        }

        void initWindowContext(const Monitor *const monitor) {
            auto windowWidth = static_cast<renderer::Dimension>(monitor->getWidth() / 1.5);
            auto windowHeight = static_cast<renderer::Dimension>(monitor->getHeight() / 1.5);

            io.windowContext.reset(new WindowContext());

            if (!io.windowContext->init(windowWidth, windowHeight)) {
                throw std::runtime_error("failed initialize OpenGL context");
            }

            io.windowContext->setTitle(m_app.getName());
            io.windowContext->setAsCurrent();
            io.windowContext->enableVSync();

            io.input.reset(new InputManager(getWindowContext()));
            io.input->calcSensetivity(monitor->getWidth(), monitor->getHeight(), monitor->getDpi());

            m_context.reset(new game::Context(io));
        }

        void createRenderer() {
            destroyRenderer();

            auto size = io.windowContext->getSize();

            renderer::Params rendererParams;
            rendererParams.setWidth(size.width);
            rendererParams.setHeight(size.height);
            io.renderer.reset(renderer::create(rendererParams));

            io.windowContext->onResize.connect([this](int width, int height) {
                console::info("resize: %i, %i", width, height);
//            renderer->setViewSize(static_cast<renderer::Dimension>(width), static_cast<renderer::Dimension>(height));
//            renderer->regenerateBuffer();
//                world->setRenderSize(width, height);
            });
        }

        void destroyRenderer() {
            if (io.renderer) {
                io.renderer->destroy();
                io.renderer.reset();
            }
        }

        WindowContext& getWindowContext() {
            return *io.windowContext;
        }

        InputManager& getInput() {
            return *io.input;
        }

        World& createWorld() {
            m_worlds.emplace_back(*m_context.get());

            return m_worlds.back();
        }

        void loadStory(const std::string& storyName, const std::string& chapterName) {
            std::thread thread([this, storyName, chapterName]() {
                std::shared_ptr<Assets> assets(new Assets(m_app.getPaths()));
                assets->loadDefaultResources();
//            world->importAssets(worldAssets);

                const resources::File storyFile = app().resource(storyName + "/story.yaml");

                StoryImporter storyImporter;
                Story story = storyImporter.import(storyFile);

                bool statusChapterLoad = storyImporter.loadChapterAssets(story, chapterName, assets.get());

                if (!statusChapterLoad) {
                    console::err("failed load chapter: %s", chapterName);
                    return;
                }
            });

            thread.join();
        }

        App& app() const {
            return m_app;
        }

    private:
        App& m_app;
        std::list<World> m_worlds;
        std::shared_ptr<Context> m_context;

        InputOutput io;
    };
}

#endif //ACTIO_ENGINE_H
