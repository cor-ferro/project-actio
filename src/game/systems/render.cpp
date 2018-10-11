#include "render.h"
#include "../world.h"
#include "../components/meshes.h"

#include "../../renderer/init.h"

namespace game {
    namespace systems {
        Render::Render(World *world) : systems::BaseSystem(world) {}

        void Render::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
            if (renderer == nullptr) return;

            processShaders();
            processTextures();
            processMeshes();

            renderer::FrameContext frameContext = renderer->createFrameContext(es);

            entityx::ComponentHandle<game::components::Camera> camera;
            entityx::ComponentHandle<game::components::Transform> cameraTransform;
            entityx::ComponentHandle<game::components::Target> cameraTarget;

            for (ex::Entity entity : es.entities_with_components(camera, cameraTransform, cameraTarget)) {
                frameContext.setView(camera->getView());
                frameContext.setProjection(camera->getProjection());
                frameContext.setCameraPosition(camera->getPosition());
                frameContext.setCameraRotation(camera->getRotation());
                frameContext.setCameraRotationMat(camera->getRotationMat());
                break;
            }

            renderer->draw(frameContext);
        }

        void Render::configure(entityx::EventManager &event_manager) {
            event_manager.subscribe<events::RenderSetupModel>(*this);
            event_manager.subscribe<events::RenderResize>(*this);
            event_manager.subscribe<ex::EntityDestroyedEvent>(*this);
        }

        void Render::setRenderer(renderer::Renderer *newRenderer) {
            renderer = newRenderer;

            // @todo: handle resize
            const renderer::Params &params = renderer->getParams();
            context->windowHeight = static_cast<float>(params.getHeight());
            context->windowWidth = static_cast<float>(params.getWidth());
        }

        void Render::receive(const events::RenderResize &event) {
            context->windowWidth = static_cast<float>(event.width);
            context->windowHeight = static_cast<float>(event.height);
        }

        void Render::receive(const events::RenderSetupModel &event) {
            ex::Entity entity = event.entity;
            events::RenderSetupModel::Action eventAction = event.action;

            ex::ComponentHandle<c::Meshes> meshes = components::get<c::Meshes>(entity);

            if (meshes) {
                switch (eventAction) {
                    case events::RenderSetupModel::Action::Add:
                        for (auto &mesh : meshes->items()) {
                            addMesh(mesh);
                        }
                        entity.assign<c::Renderable>();
                        break;
                    case events::RenderSetupModel::Action::Update:
                        for (auto &mesh : meshes->items()) {
                            updateMesh(mesh);
                        }
                        break;
                    case events::RenderSetupModel::Action::Remove:
                        for (auto &mesh : meshes->items()) {
                            removeMesh(mesh);
                        }
                        entity.remove<c::Renderable>();
                }
            }
        }

        void Render::receive(const ex::EntityDestroyedEvent &event) {
            ex::Entity entity = event.entity;

            auto meshes = components::get<c::Meshes>(entity);

            if (meshes) {
                for (auto &mesh : meshes->items()) {
                    removeMesh(mesh);
                }
                entity.remove<c::Renderable>();
            }
        }

        void Render::addMesh(std::shared_ptr<Mesh> &mesh) {
            setupMesh.push(std::make_tuple(MeshAction::Add, mesh));
        }

        void Render::updateMesh(std::shared_ptr<Mesh> &mesh) {
            setupMesh.push(std::make_tuple(MeshAction::Update, mesh));
        }

        void Render::removeMesh(std::shared_ptr<Mesh> &mesh) {
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
                std::tuple<MeshAction, std::shared_ptr<Mesh>> &item = setupMesh.top();

                MeshAction action = std::get<0>(item);
                std::shared_ptr<Mesh> mesh = std::get<1>(item);

                switch (action) {
                    case MeshAction::Add:
                        renderer->createMesh(mesh);
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