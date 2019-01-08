#include "loader.h"
#include "../story_importer.h"
#include "../../app/app.h"
#include "../events/load_story_complete.h"

namespace game {
    class World;

    namespace systems {
        Loader::Loader(Context &context) : systems::BaseSystem(context) {

        }

        void Loader::configure(ex::EntityManager &es, entityx::EventManager &events) {
            events.subscribe<events::LoadStory>(*this);
        }

        void Loader::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
            processQueue<StoryItem>(storyItems, [&es, &events, this](StoryItem& item) {
                auto &assets = m_context.getAssets();
                assets.reset(new Assets(App::instance()->getPaths()));

                const resources::File storyFile = App::instance()->resource(item.storyName + "/story.yaml");

                StoryImporter storyImporter;
                Story story = storyImporter.import(storyFile);

                bool statusChapterLoad = storyImporter.loadChapterAssets(story, item.chapterName, assets.get());

                if (!statusChapterLoad) {
                    console::err("failed load chapter: %s", item.chapterName);
                    return;
                }



                events.emit<events::LoadStoryComplete>(item.storyName, item.chapterName);
            });
        }

        void Loader::receive(const events::LoadStory &event) {
            storyItems.push(StoryItem{event.storyName, event.chapterName});
        }
    }
}