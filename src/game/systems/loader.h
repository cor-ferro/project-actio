
#ifndef ACTIO_SYSTEMS_LOADER_H
#define ACTIO_SYSTEMS_LOADER_H

#include <entityx/entityx/System.h>
#include "../events/load_story.h"
#include "../context.h"
#include "base.h"

namespace game {
    class World;

    namespace systems {
        namespace ex = entityx;

        class Loader
                : public systems::BaseSystem
                        , public entityx::System<Loader>
                        , public entityx::Receiver<Loader> {
        public:
            explicit Loader(Context& context);

            void configure(ex::EntityManager &es, entityx::EventManager &events) override;

            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;

            void receive(const events::LoadStory& event);

        private:
            void loadStory();

            struct StoryItem {
                std::string storyName;
                std::string chapterName;
            };

            std::stack<StoryItem> storyItems;
        };
    }
}

#endif //ACTIO_SYSTEMS_LOADER_H
