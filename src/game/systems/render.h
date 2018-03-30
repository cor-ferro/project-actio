#ifndef GAME_SYSTEMS_RENDER_H_
#define GAME_SYSTEMS_RENDER_H_

#include <unordered_map>
#include <vector>
#include <chrono>
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
#include "../../cameras/camera.h"

namespace game {
    namespace systems {
        using namespace entityx;

        class Render : public entityx::System<Render> {
        public:
            explicit Render(renderer::Renderer *newRenderer) {
                renderer = newRenderer;
            }

            void update(EntityManager &es, EventManager &events, TimeDelta dt) override {
                renderer->draw(es);
            }

        private:
            renderer::Renderer *renderer = nullptr;
        };
    }
}

#endif