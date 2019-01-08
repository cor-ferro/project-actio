#ifndef ACTIO_WORLD_H
#define ACTIO_WORLD_H

#include <stack>
#include <entityx/entityx/Event.h>
#include <entityx/entityx/config.h>
#include <entityx/entityx/System.h>

#include "base.h"
#include "../events/load_story_complete.h"

namespace game {
    namespace systems {
        namespace ex = entityx;
        namespace c = components;

        class World : public systems::BaseSystem, public entityx::System<World>, public ex::Receiver<World> {
        public:
            explicit World(Context& context);

            ~World() override;

            void configure(ex::EntityManager &entities, entityx::EventManager &event_manager) override;

            void start(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt);

            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;

            void receive(const events::LoadStoryComplete& event);

        private:
            struct Story {};

            std::stack<Story> loadedStory;
        };
    }
}

#endif //ACTIO_WORLD_H
