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

        void setup();

        void start();

        void update(const ex::TimeDelta& dt);

        void render(const ex::TimeDelta& dt);

        void setRenderer(std::shared_ptr<renderer::Renderer>& renderer);

        ex::Entity createEntity();

    private:
        profiling::ProfileTimings systemsProfiler;

        Context& m_context;
    };
}

#endif //ACTIO_WORLD_H
