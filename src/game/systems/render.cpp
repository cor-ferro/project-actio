#include "render.h"
#include "../world.h"

namespace game {
    namespace systems {
        Render::Render(World *world, renderer::Renderer *newRenderer)
                : renderer(newRenderer)
                , systems::BaseSystem(world) {}

        void Render::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
            if (renderer == nullptr) return;

            processShaders();
            processTextures();
            processMeshes();

            renderer->draw(es);
        }

        void Render::configure(entityx::EventManager &event_manager) {
            event_manager.subscribe<events::RenderSetupModel>(*this);
            event_manager.subscribe<events::RenderResize>(*this);
            event_manager.subscribe<ex::EntityDestroyedEvent>(*this);

            // @todo: handle resize
            renderer::Params params = renderer->getParams();
            context->windowHeight = static_cast<float>(params.height);
            context->windowWidth = static_cast<float>(params.width);
        }

        void Render::receive(const events::RenderResize &event) {
            context->windowWidth = static_cast<float>(event.width);
            context->windowHeight = static_cast<float>(event.height);
        }

        void Render::receive(const events::RenderSetupModel &event) {
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

        void Render::receive(const ex::EntityDestroyedEvent &event) {
            ex::Entity entity = event.entity;

            auto model = components::get<c::Model>(entity);

            if (model) {
                removeModel(model.get());
                entity.remove<c::Renderable>();
            }
        }

        void Render::addModel(Model *model) {
            auto meshes = model->getMeshes();

            for (auto &mesh : meshes) {
                addMesh(mesh);
            }
        }

        void Render::updateModel(Model *model) {
            auto meshes = model->getMeshes();

            for (auto &mesh : meshes) {
                updateMesh(mesh);
            }
        }

        void Render::removeModel(Model *model) {
            auto meshes = model->getMeshes();

            for (auto &mesh : meshes) {
                removeMesh(mesh);
            }
        }

        void Render::addMesh(Mesh *mesh) {
            setupMesh.push(std::make_tuple(MeshAction::Add, mesh));
        }

        void Render::updateMesh(Mesh *mesh) {
            setupMesh.push(std::make_tuple(MeshAction::Update, mesh));
        }

        void Render::removeMesh(Mesh *mesh) {
            setupMesh.push(std::make_tuple(MeshAction::Remove, mesh));
        }

        void Render::addShader(assets::Shader *asset) {
            setupShaders.push(asset);
        }

        void Render::addTexture(assets::Texture *asset) {
            setupTextures.push({ asset, TextureAction::Add });
        }

        void Render::removeTexture(assets::Texture *asset) {
            setupTextures.push({ asset, TextureAction::Remove });
        }

        void Render::addMaterial(assets::Material *asset) {
            setupMaterials.push(asset);
        }

        void Render::destroy() {
            renderer->destroy();
            renderer = nullptr;
        }

        void Render::processShaders() {
            while (!setupShaders.empty()) {
                assets::Shader *asset = setupShaders.top();
                setupShaders.pop();
            }
        }

        void Render::processTextures() {
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

        void Render::processMeshes() {
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
    }
}