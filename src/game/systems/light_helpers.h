//
// Created by demitriy on 4/17/18.
//

#ifndef ACTIO_LIGHT_HELPERS_H
#define ACTIO_LIGHT_HELPERS_H

#include <entityx/entityx/System.h>
#include <stack>
#include "../events/light_add.h"
#include "../events/light_remove.h"
#include "../events/light_helper_show.h"
#include "../events/render_setup_model.h"
#include "../components/light_point.h"
#include "../components/model.h"
#include "../components/base.h"
#include "../components/transform.h"
#include "../components/light_helper.h"
#include "../components/renderable.h"
#include "../components/helper.h"
#include "../../core/geometry_builder.h"
#include "../../core/mesh.h"
#include "../context.h"
#include "base.h"

namespace game {
    class World;

    namespace systems {
        namespace ex = entityx;

        class LightHelpers
                : public systems::BaseSystem
                  , public entityx::System<LightHelpers>
                  , public entityx::Receiver<LightHelpers> {
        public:
            explicit LightHelpers(World *world);

            void configure(entityx::EventManager &event_manager) override;

            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;

            void receive(const events::LightAdd &event);

            void receive(const events::LightRemove &event);

            void receive(const events::LightHelperShow &event);

        private:
            void showHelpers(ex::EntityManager &es);

            void hideHelpers(ex::EntityManager &es);

            void updateHelperPositions(ex::EntityManager &es);

            std::stack<entityx::Entity> newEntities;
            bool isChangeStatus = false;
            bool isShowHelpers = false;
        };
    }
}

#endif //ACTIO_LIGHT_HELPERS_H
