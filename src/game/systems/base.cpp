#include "base.h"
#include "../../lib/console.h"

namespace game {
    struct World;

    namespace systems {
        namespace ex = entityx;

        BaseSystem::BaseSystem(Context& context) : m_context(context) {}

        void BaseSystem::start(ex::EntityManager& es, ex::EventManager& events, ex::TimeDelta dt) {
            console::info("start system");
        }

        void BaseSystem::setMaxItemsPerQueue(int count) {
            maxItemsPerQueue = count;
        }

        const int BaseSystem::getMaxItemsPerQueue() const {
            return maxItemsPerQueue;
        }
    }
}