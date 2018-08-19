//
// Created by demitriy on 3/30/18.
//

#ifndef ACTIO_BALL_SHOT_H
#define ACTIO_BALL_SHOT_H

#include <entityx/entityx/System.h>
#include <stack>
#include <glm/glm/gtc/random.inl>
#include "../components/physic.h"
#include "../components/transform.h"
#include "../components/camera.h"
#include "../components/model.h"
#include "../components/renderable.h"
#include "../components/user_control.h"
#include "../events/key_press.h"
#include "../events/physic_create.h"
#include "../events/render_setup_model.h"
#include "../events/mouse_press.h"
#include "../events/physic_force.h"
#include "../../lib/console.h"
#include "../../lib/types.h"
#include "../../core/mesh.h"
#include "../../core/geometry_builder.h"
#include "../context.h"
#include "base.h"

namespace game {
    class World;

    namespace systems {
        namespace ex = entityx;
        namespace c = components;

        class BallShot
                : public systems::BaseSystem
                  , public ex::System<BallShot>
                  , public ex::Receiver<BallShot> {
        public:
            explicit BallShot(World *world);

            void configure(ex::EventManager &event_manager) override;

            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;

            void receive(const events::KeyPress &event);

            void receive(const events::MousePress &event);

        private:
            std::stack<int> newItems;
        };
    }
}

#endif //ACTIO_BALL_SHOT_H
