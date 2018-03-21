#ifndef GAME_SYSTEMS_RENDER_H_
#define GAME_SYSTEMS_RENDER_H_

#include "entityx/entityx.h"
#include "../components/renderable.h"
#include "../../lib/console.h"
#include "../../scenes/scene.h"
#include "../../resources/resources.h"

namespace game {
    namespace systems {
        using namespace entityx;

        class Render : public entityx::System<Render> {
        public:
            Render() {
                scene = new Scene();
            }

            void update(EntityManager &es, EventManager &events, TimeDelta dt) override {
                // console::info("render");
                es.each<components::Renderable>([](Entity entity, components::Renderable&) {
                    
                });
            }
        private:
            Scene* scene;
        };
    }
}

#endif