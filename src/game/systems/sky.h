//
// Created by demitriy on 7/8/18.
//

#ifndef ACTIO_GAME_SYSTEMS_SKY_H
#define ACTIO_GAME_SYSTEMS_SKY_H

#include <entityx/entityx/System.h>
#include "../components/model.h"
#include "../components/transform.h"
#include "../events/render_setup_model.h"
#include "../../core/mesh.h"
#include "../../core/geometry_primitive.h"
#include "../../lib/console.h"
#include "base.h"

namespace game {
    class World;

    namespace systems {
        namespace ex = entityx;
        namespace c = components;

        class Sky
                : public systems::BaseSystem
                  , public entityx::System<Sky>
                  , public entityx::Receiver<Sky> {
        public:
            explicit Sky(World *world);

            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;

        private:
            bool isFirstCall = false;
        };
    }
}

#endif //ACTIO_GAME_SYSTEMS_SKY_H
