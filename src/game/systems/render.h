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
#include "../events/render_resize.h"
#include "../events/render_setup_model.h"
#include "../../lib/console.h"
#include "../../lib/assets_types.h"
#include "../../resources/resources.h"
#include "../../renderer/renderer.h"
#include "../../lights/direction_light.h"
#include "../../lights/point_light.h"
#include "../../lights/spot_light.h"
#include "../../cameras/camera.h"
#include "../../core/mesh.h"
#include "../../core/model.h"
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
            enum class ModelAction {
                Add = 1,
                Remove = 2,
                Update = 3
            };

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
                processMeshes();

                renderer->draw(es);
            }

            void configure(entityx::EventManager &event_manager) override {
                event_manager.subscribe<events::RenderSetupModel>(*this);
                event_manager.subscribe<events::RenderResize>(*this);
                event_manager.subscribe<ex::EntityDestroyedEvent>(*this);

                // @todo: handle resize
                renderer::Params params = renderer->getParams();
                worldContext->windowHeight = static_cast<float>(params.height);
                worldContext->windowWidth = static_cast<float>(params.width);
            }

            void receive(const events::RenderResize &event) {
                worldContext->windowWidth = static_cast<float>(event.width);
                worldContext->windowHeight = static_cast<float>(event.height);
            }

            void receive(const events::RenderSetupModel &event) {
                ex::Entity entity = event.entity;
                events::RenderSetupModel::Action eventAction = event.action;

                ex::ComponentHandle<c::Model> model = components::get<c::Model>(entity);

                if (model) {
                    switch (eventAction) {
                        case events::RenderSetupModel::Action::Add:
                            addModel(model.get());
                            entity.assign<c::Renderable>();
                            break;
                        case events::RenderSetupModel::Action::Update:
                            updateModel(model.get());
                            break;
                        case events::RenderSetupModel::Action::Remove:
                            removeModel(model.get());
                            entity.remove<c::Renderable>();
                    }
                }
            }

            void receive(const ex::EntityDestroyedEvent &event) {
                ex::Entity entity = event.entity;

                auto model = components::get<c::Model>(entity);

                if (model) {
                    removeModel(model.get());
                    entity.remove<c::Renderable>();
                }
            }

            void addModel(Model *model) {
                auto meshes = model->getMeshes();

                for (auto &mesh : meshes) {
                    addMesh(mesh);
                }
            }

            void updateModel(Model *model) {
                auto meshes = model->getMeshes();

                for (auto &mesh : meshes) {
                    updateMesh(mesh);
                }
            }

            void removeModel(Model *model) {
                auto meshes = model->getMeshes();

                for (auto &mesh : meshes) {
                    removeMesh(mesh);
                }
            }

            void addMesh(Mesh *mesh) {
                setupMesh.push(std::make_tuple(MeshAction::Add, mesh));
            }

            void updateMesh(Mesh *mesh) {
                setupMesh.push(std::make_tuple(MeshAction::Update, mesh));
            }

            void removeMesh(Mesh *mesh) {
                setupMesh.push(std::make_tuple(MeshAction::Remove, mesh));
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
            int updatePerTick = 30;

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

            void processMeshes() {
                size_t counter = 0;

                while (!setupMesh.empty() && counter < updatePerTick) {
                    std::tuple<MeshAction, Mesh *> &item = setupMesh.top();

                    MeshAction action = std::get<0>(item);
                    Mesh *mesh = std::get<1>(item);

                    switch (action) {
                        case MeshAction::Add:
                            renderer->setupMesh(mesh);
                            break;
                        case MeshAction::Remove:
                            renderer->destroyMesh(mesh);
                            break;
                        case MeshAction ::Update:
                            renderer->updateMesh(mesh);
                            break;
                        default:
                            console::warn("unknown mesh action");
                    }

                    setupMesh.pop();
                    counter++;
                }
            }

            renderer::Renderer *renderer = nullptr;

            std::stack<std::tuple<MeshAction, Mesh *>> setupMesh;
            std::stack<assets::Shader*> setupShaders;
            std::stack<std::pair<assets::Texture*, TextureAction>> setupTextures;
            std::stack<assets::Material*> setupMaterials;
        };
    }
}

#endif