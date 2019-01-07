#ifndef ACTIO_LOAD_STORY_H
#define ACTIO_LOAD_STORY_H

#include "../../lib/task.h"
#include "../../app/app.h"
#include "../context.h"
#include <string>

namespace game {
    class TaskLoadStory : public Task {
    public:
        TaskLoadStory(const App& app, const game::Context& gameContext)
            : m_app(app)
            , m_gameContext(gameContext)
            {};

        ~TaskLoadStory() override = default;

        void onStart() override;

        void onFinish() override;

        void onFlush() override;

        void onFail() override;

        const App &getApp() const;

        const Context &getGameContext() const;

        const std::string &getStoryName() const;

        void setStoryName(const std::string &storyName);

        const std::string &getChapterName() const;

        void setChapterName(const std::string &chapterName);

    private:


        const App &m_app;
        const game::Context &m_gameContext;
        std::string m_storyName;
        std::string m_chapterName;
    };
}

#endif //ACTIO_LOAD_STORY_H
