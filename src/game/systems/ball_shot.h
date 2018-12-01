//
// Created by demitriy on 3/30/18.
//

#ifndef ACTIO_BALL_SHOT_H
#define ACTIO_BALL_SHOT_H

#include <entityx/entityx/System.h>
#include <stack>
#include <glm/glm/gtc/random.inl>
#include "../components/physic_actor.h"
#include "../components/transform.h"
#include "../components/camera.h"
#include "../components/model.h"
#include "../components/renderable.h"
#include "../components/user_control.h"
#include "../components/mesh.h"
#include "../events/key_press.h"
#include "../events/render_update_mesh.h"
#include "../events/mouse_press.h"
#include "../events/physic_force.h"
#include "../events/object_create.h"
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
            explicit BallShot(Context& context);

            void configure(ex::EventManager &events) override;

            void start(ex::EntityManager& es, ex::EventManager& events, ex::TimeDelta dt);

            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;

            void receive(const events::KeyPress &event);

        private:
            struct BallDescription {
                float radius = 1.0f;
                float pushForce = 15.0f;
            };

            std::stack<BallDescription> newBalls;
            std::shared_ptr<Mesh> ballMesh;
            std::shared_ptr<Material> ballMaterial;
        };
    }
}

#endif //ACTIO_BALL_SHOT_H
