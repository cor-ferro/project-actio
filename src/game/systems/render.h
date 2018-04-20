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
#include "../events/render_setup_mesh.h"
#include "../events/render_update_mesh.h"
#include "../context.h"
#include "base.h"

namespace game {
    namespace systems {
        namespace ex = entityx;

        class Render
                : systems::BaseSystem
                  , public entityx::System<Render>
                  , public ex::Receiver<Render> {
        public:
            explicit Render(Context *context, renderer::Renderer *newRenderer)
                    : renderer(newRenderer)
                    , systems::BaseSystem(context) {}

            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override {
                uploadGeometry();
                updateGeometry();
                renderer->draw(es);
            }

            void configure(entityx::EventManager &event_manager) override {
                event_manager.subscribe<events::RenderSetupMesh>(*this);
                event_manager.subscribe<events::RenderUpdateMesh>(*this);

                // @todo: handle resize
                renderer::Params params = renderer->getParams();
                worldContext->windowHeight = static_cast<float>(params.height);
                worldContext->windowWidth = static_cast<float>(params.width);
            }

            void receive(const events::RenderSetupMesh &event) {
                setupMesh.push({event.mesh, event.entity});
            }

            void receive(const events::RenderUpdateMesh &event) {
                updateMesh.push({event.mesh, event.entity});
            }

        private:
            void uploadGeometry() {
                int counter = 0;
                while (!setupMesh.empty() && counter < 5) {
                    std::pair<Mesh *, entityx::Entity> pair = setupMesh.top();
                    setupMesh.pop();

                    renderer->setupMesh(pair.first);
                    pair.second.assign<components::Renderable>();

                    counter++;
                }
            }

            void updateGeometry() {
                int counter = 0;
                while (!updateMesh.empty() && counter < 5) {
                    std::pair<Mesh *, entityx::Entity> pair = updateMesh.top();
                    updateMesh.pop();

                    renderer->updateMesh(pair.first);

                    counter++;
                }
            }

            renderer::Renderer *renderer = nullptr;
            std::stack<std::pair<Mesh *, entityx::Entity>> setupMesh;
            std::stack<std::pair<Mesh *, entityx::Entity>> updateMesh;
        };
    }
}

#endif