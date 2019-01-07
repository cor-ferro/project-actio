#include "load_story.h"
#include <memory>
#include "../story_importer.h"
#include "../../app/app.h"
#include "../context.h"

namespace game {
    void TaskLoadStory::onStart() {
//        void *data = m_taskContext.getData();
//        auto *asset = reinterpret_cast<assets::Model *>(data);



        auto assets = std::make_shared<Assets>(m_app.getPaths());

        const std::string& storyName = getStoryName();
        const std::string& chapterName = getChapterName();

        const resources::File storyFile = m_app.resource(storyName + "/story.yaml");

        StoryImporter storyImporter;
        Story story = storyImporter.import(storyFile);

        bool statusChapterLoad = storyImporter.loadChapterAssets(story, chapterName, assets.get());

        if (!statusChapterLoad) {
            console::err("failed load chapter: %s", chapterName);
            return;
        }
    }

    void TaskLoadStory::onFinish() {

    }

    void TaskLoadStory::onFlush() {
//            world->importAssets(worldAssets);
    }

    void TaskLoadStory::onFail() {

    }

    const App &TaskLoadStory::getApp() const {
        return m_app;
    }

    const Context &TaskLoadStory::getGameContext() const {
        return m_gameContext;
    }

    const std::string &TaskLoadStory::getStoryName() const {
        return m_storyName;
    }

    void TaskLoadStory::setStoryName(const std::string &storyName) {
        TaskLoadStory::m_storyName = storyName;
    }

    const std::string &TaskLoadStory::getChapterName() const {
        return m_chapterName;
    }

    void TaskLoadStory::setChapterName(const std::string &chapterName) {
        TaskLoadStory::m_chapterName = chapterName;
    }
}