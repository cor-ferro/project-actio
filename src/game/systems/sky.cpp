#include "sky.h"

namespace game {
    namespace systems {
        Sky::Sky(Context& context) : systems::BaseSystem(context) {}

        void Sky::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {}
    }
}