#include "base.h"
#include "../world.h"

namespace game {
    struct World;

    namespace systems {
        namespace ex = entityx;

        BaseSystem::BaseSystem(game::World *world)
            : world(world)
            , context(&world->getContext())
        {}
    }
}