#ifndef GAME_SYSTEMS_RENDER_H_
#define GAME_SYSTEMS_RENDER_H_

#include <unordered_map>
#include <vector>
#include <chrono>
#include <stack>
#include "entityx/entityx.h"
#include "../components/base.h"
#include "../components/renderable.h"
#include "../components/transform.h"
#include "../components/model.h"
#include "../events/render_create_mesh.h"
#include "../events/render_update_mesh.h"
#include "../events/render_resize.h"
#include "../../lib/console.h"
#include "../../lib/assets_types.h"
#include "../../resources/resources.h"
#include "../../renderer/renderer.h"
#include "../../lights/direction_light.h"
#include "../../lights/point_light.h"
#include "../../lights/spot_light.h"
#include "../../cameras/camera.h"
#include "../context.h"
#include "base.h"

namespace game {
    namespace systems {
        namespace ex = entityx;
        namespace c = components;

        class Render
                : systems::BaseSystem
                  , public entityx::System<Render>
                  , public ex::Receiver<Render> {
        private:
            enum class MeshAction {
                Add = 1,
                Remove = 2,
                Update = 3
            };

            enum class TextureAction {
                Add = 1,
                Remove = 2
            };

            enum class ShaderAction {
                Add = 1,
                Remove = 2
            };

        public:
            explicit Render(Context *context, renderer::Renderer *newRenderer)
                    : renderer(newRenderer)
                    , systems::BaseSystem(context) {}

            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override {
                if (renderer == nullptr) return;

                processShaders();
                processTextures();
                processModels();
                processMeshes();
                processGeometry();
                updateGeometry();
                destroyGeometry();
                renderer->draw(es);
            }

            void configure(entityx::EventManager &event_manager) override {
                event_manager.subscribe<events::RenderCreateMesh>(*this);
                event_manager.subscribe<events::RenderUpdateMesh>(*this);
                event_manager.subscribe<events::RenderResize>(*this);
                event_manager.subscribe<ex::EntityDestroyedEvent>(*this);

                // @todo: handle resize
                renderer::Params params = renderer->getParams();
                worldContext->windowHeight = static_cast<float>(params.height);
                worldContext->windowWidth = static_cast<float>(params.width);
            }

            void receive(const events::RenderCreateMesh &event) {
                createMesh.push({event.mesh, event.entity});
            }

            void receive(const events::RenderUpdateMesh &event) {
                updateMesh.push({event.mesh, event.entity});
            }

            void receive(const events::RenderResize &event) {
                worldContext->windowWidth = static_cast<float>(event.width);
                worldContext->windowHeight = static_cast<float>(event.height);
            }

            void receive(const ex::EntityDestroyedEvent &event) {
                ex::Entity entity = event.entity;

                setupModel.push(entity);
            }

            void addMesh(Mesh *mesh) {

            }

            void addShader(assets::Shader *asset) {
                setupShaders.push(asset);
            }

            void addTexture(assets::Texture *asset) {
                setupTextures.push({ asset, TextureAction::Add });
            }

            void removeTexture(assets::Texture *asset) {
                setupTextures.push({ asset, TextureAction::Remove });
            }

            void addMaterial(assets::Material *asset) {
                setupMaterials.push(asset);
            }

            void destroy() {
                renderer->destroy();
                renderer = nullptr;
            }

        private:
            void processShaders() {
                while (!setupShaders.empty()) {
                    assets::Shader *asset = setupShaders.top();
                    setupShaders.pop();


                }
            }

            void processTextures() {
                while (!setupTextures.empty()) {
                    std::pair<assets::Texture*, TextureAction> &process = setupTextures.top();
                    setupTextures.pop();

                    assets::Texture *asset = process.first;
                    TextureAction &action = process.second;

                    if (action == TextureAction::Add) {
//                        renderer->createTexture(asset->getImage());
                    } else if (action == TextureAction::Remove) {
//                        renderer->destroyTexture();
                    } else {
                        console::warn("unknown process texture action");
                    }
                }
            }

            void processGeometry() {
                int counter = 0;
                while (!createMesh.empty() && counter < uploadPerTick) {
                    std::pair<Mesh *, entityx::Entity> pair = createMesh.top();
                    createMesh.pop();

                    renderer->setupMesh(pair.first);
                    pair.second.assign<components::Renderable>();

                    counter++;
                }
            }

            void updateGeometry() {
                int counter = 0;
                while (!updateMesh.empty() && counter < updatePerTick) {
                    std::pair<Mesh *, entityx::Entity> pair = updateMesh.top();
                    updateMesh.pop();

                    renderer->updateMesh(pair.first);

                    counter++;
                }
            }

            void destroyGeometry() {
                while (!destroyMesh.empty()) {
                    std::pair<Mesh *, entityx::Entity> pair = destroyMesh.top();
                    destroyMesh.pop();

                    renderer->destroyMesh(pair.first);
                }
            }

            void processModels() {
                while (!setupModel.empty()) {
                    ex::Entity entity = setupModel.top();
                    setupModel.pop();

                    ex::ComponentHandle<c::Renderable> renderable = components::get<c::Renderable>(entity);

                    if (renderable) {
                        ex::ComponentHandle<c::Model> model = components::get<c::Model>(entity);

                        if (model) {
                            auto meshes = model->getMeshes();

                            for (auto &mesh : meshes) {
                                renderer->setupMesh(mesh);
                            }
                        }
                    }
                }
            }

            void processMeshes() {
                while (!setupMesh.empty()) {
                    std::tuple<Mesh *, ex::Entity, MeshAction> &item = setupMesh.top();

                    Mesh *mesh = std::get<0>(item);
                    ex::Entity entity = std::get<1>(item);
                    MeshAction action = std::get<2>(item);

                    switch (action) {
                        case MeshAction::Add:
                            renderer->setupMesh(mesh);
                            break;
                        case MeshAction::Remove:
                            renderer->destroyMesh(mesh);
                    }

                    setupMesh.pop();
                }
            }

            int uploadPerTick = 30;
            int updatePerTick = 30;

            renderer::Renderer *renderer = nullptr;
            std::stack<entityx::Entity> setupModel;
            std::stack<std::pair<Mesh *, ex::Entity>> createMesh;
            std::stack<std::pair<Mesh *, ex::Entity>> destroyMesh;
            std::stack<std::pair<Mesh *, ex::Entity>> updateMesh;

            std::stack<std::tuple<Mesh *, ex::Entity, MeshAction>> setupMesh;
            std::stack<assets::Shader*> setupShaders;
            std::stack<std::pair<assets::Texture*, TextureAction>> setupTextures;
            std::stack<assets::Material*> setupMaterials;
        };
    }
}

#endif