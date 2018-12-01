//
// Created by demitriy on 11/9/18.
//

#ifndef ACTIO_GAME_WORLD_H
#define ACTIO_GAME_WORLD_H

#include <entityx/entityx.h>

#include "context.h"

#include "systems/physic.h"
#include "systems/camera.h"
#include "systems/render.h"
#include "systems/ball_shot.h"
#include "systems/animations.h"
#include "systems/character_control.h"
#include "systems/input.h"
#include "systems/char_control.h"
#include "systems/lights.h"
#include "systems/light_helpers.h"
#include "systems/day_time.h"
#include "systems/weapons.h"
#include "systems/sky.h"

#include "../lib/profiling.h"

namespace game {
    namespace ex = entityx;

    class World : ex::EntityX {
    public:
        explicit World(Context& context) : m_context(context) {};

        void setup() {
            systems.add<systems::Render>(m_context);
            systems.add<systems::Input>(m_context);
            systems.add<systems::Camera>(m_context);
            systems.add<systems::CharControl>(m_context);
            systems.add<systems::Animations>(m_context);
            systems.add<systems::Physic>(m_context);
            systems.add<systems::BallShot>(m_context);
            systems.add<systems::Lights>(m_context);
            systems.add<systems::LightHelpers>(m_context);
            systems.add<systems::DayTime>(m_context);
            systems.add<systems::Sky>(m_context);
            systems.add<systems::Weapons>(m_context);

            systems.configure();
        }

        void start() {
            ex::TimeDelta dt = 0.16;

            systems.system<systems::Render>()->start(entities, events, dt);
            systems.system<systems::Input>()->start(entities, events, dt);
            systems.system<systems::Camera>()->start(entities, events, dt);
            systems.system<systems::CharControl>()->start(entities, events, dt);
            systems.system<systems::Animations>()->start(entities, events, dt);
            systems.system<systems::Physic>()->start(entities, events, dt);
            systems.system<systems::BallShot>()->start(entities, events, dt);
            systems.system<systems::Lights>()->start(entities, events, dt);
            systems.system<systems::LightHelpers>()->start(entities, events, dt);
            systems.system<systems::DayTime>()->start(entities, events, dt);
            systems.system<systems::Sky>()->start(entities, events, dt);
            systems.system<systems::Weapons>()->start(entities, events, dt);
        }

        void update(const ex::TimeDelta& dt) {
            profiling::Mark m = profiling::mark(systemsProfiler, "EngineUpdate");

            // pre update
            PROFILE(systemsProfiler, "Input", systems.update<game::systems::Input>(dt));
            PROFILE(systemsProfiler, "Camera", systems.update<game::systems::Camera>(dt));
            PROFILE(systemsProfiler, "CharControl", systems.update<game::systems::CharControl>(dt));
            PROFILE(systemsProfiler, "Physic", systems.update<game::systems::Physic>(dt));
            PROFILE(systemsProfiler, "Lights", systems.update<game::systems::Lights>(dt));
            PROFILE(systemsProfiler, "DayTime", systems.update<game::systems::DayTime>(dt));
            PROFILE(systemsProfiler, "Sky", systems.update<game::systems::Sky>(dt));

            // main update
            PROFILE(systemsProfiler, "Weapons", systems.update<game::systems::Weapons>(dt));
            PROFILE(systemsProfiler, "BallShot", systems.update<game::systems::BallShot>(dt));

//    for (auto &script : scripts) {
//        PROFILE(systemProfiler, script->getName(), script->evalUpdate(static_cast<float>(dt)));
//    }

            // post update
            PROFILE(systemsProfiler, "Animations", systems.update<game::systems::Animations>(dt));
            PROFILE(systemsProfiler, "LightHelpers", systems.update<game::systems::LightHelpers>(dt));
        }

        void render(const ex::TimeDelta& dt) {
            PROFILE(systemsProfiler, "Render", systems.update<game::systems::Render>(dt));
        }

        ex::Entity createEntity() {
            return entities.create();
        }

    private:
        profiling::ProfileTimings systemsProfiler;

        Context& m_context;
    };
}

#endif //ACTIO_WORLD_H
