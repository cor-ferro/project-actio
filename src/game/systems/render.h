#ifndef GAME_SYSTEMS_RENDER_H_
#define GAME_SYSTEMS_RENDER_H_

#include <unordered_map>
#include <vector>
#include <chrono>
#include <stack>
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
#include "../events/render_setup_geometry.h"

namespace game {
    namespace systems {
        using namespace entityx;

        class Render : public entityx::System<Render>, public Receiver<Render> {
        public:
            explicit Render(renderer::Renderer *newRenderer) {
                renderer = newRenderer;
            }

            void update(EntityManager &es, EventManager &events, TimeDelta dt) override {
                uploadGeometry();
                renderer->draw(es);
            }

            void configure(entityx::EventManager &event_manager) override {
                event_manager.subscribe<events::RenderSetupMesh>(*this);
            }

            void receive(const events::RenderSetupMesh &event) {
                setupMesh.push({ event.mesh, event.entity });
            }

        private:
            void uploadGeometry() {
                int counter = 0;
                while (!setupMesh.empty() && counter < 5) {
                    std::pair<Mesh*, entityx::Entity> pair = setupMesh.top();
                    setupMesh.pop();

                    renderer->setupMesh(pair.first);
                    pair.second.assign<components::Renderable>();

                    counter++;
                }
            }

            renderer::Renderer *renderer = nullptr;
            std::stack<std::pair<Mesh*, entityx::Entity>> setupMesh;
        };
    }
}

#endif