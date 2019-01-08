#ifndef ACTIO_ENGINE_H
#define ACTIO_ENGINE_H

#include <memory>
#include <list>
#include <string>
#include <thread>
#include <stdexcept>
#include <queue>

#include "input_output.h"
#include "../renderer/init.h"
#include "../app/app.h"
#include "world.h"
#include "context.h"
#include "story_importer.h"
#include "../lib/input_manager.h"
#include "../lib/task.h"
#include "../lib/task_manager.h"

namespace game {
    namespace ex = entityx;

    class Engine {
    public:
        explicit Engine(App &app);

        ~Engine();

        void setupWorlds();

        void startWorlds();

        void startLoadStory(World& world, const std::string &storyName, const std::string &chapterName);

        void update();

        void updateWorlds(float elapsedTime);

        void destroyWorlds();

        void render(const ex::TimeDelta &dt);

        void reset();

        void enableDebug();

        void disableDebug();

        void initWindowContext(const Monitor *const monitor);

        void createRenderer();

        void destroyRenderer();

        WindowContext &getWindowContext();

        InputManager &getInput();

        World &createWorld();

        App &app() const;

    private:
        App &m_app;
        std::list<World> m_worlds;
        std::shared_ptr<Context> m_context;

        InputOutput io;
        TasksManager m_tasksManager;
    };
}

#endif //ACTIO_ENGINE_H
