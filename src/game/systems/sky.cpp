#include "sky.h"
#include "../world.h"

namespace game {
    namespace systems {
        Sky::Sky(World *world) : systems::BaseSystem(world) {}

        void Sky::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {}
    }
}