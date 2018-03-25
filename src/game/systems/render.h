#ifndef GAME_SYSTEMS_RENDER_H_
#define GAME_SYSTEMS_RENDER_H_

#include <unordered_map>
#include <vector>
#include "entityx/entityx.h"
#include "../components/renderable.h"
#include "../components/transform.h"
#include "../components/model.h"
#include "../../lib/console.h"
#include "../../resources/resources.h"
#include "../../renderer/renderer.h"
#include "../../lights/direction_light.h"
#include "../../lights/point_light.h"
#include "../../lights/spot_light.h"

namespace game {
    namespace systems {
        using namespace entityx;

        class Render : public entityx::System<Render> {
        public:
            Render(renderer::Renderer *newRenderer) {
                renderer = newRenderer;
            }

            void update(EntityManager &es, EventManager &events, TimeDelta dt) override {
                // console::info("render");
                es.each<components::Renderable, components::Transform, components::Model>([](
                        Entity entity,
                        components::Renderable &,
                        components::Transform &transform,
                        components::Model &model
                ) {
                    console::info("render entity");
                });
            }

        private:
            renderer::Renderer *renderer;
            std::vector<Light::Directional> directionalLights;
            std::vector<Light::Point> pointLights;
            std::vector<Light::Spot> spotLights;
        };
    }
}

#endif