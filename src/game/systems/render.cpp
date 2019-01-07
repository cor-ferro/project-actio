#include "render.h"
#include "../components/mesh.h"

#include "../../renderer/init.h"
#include "../components/camera.h"
#include "../components/target.h"

#include "../../imgui_impl_glfw_gl3.h"

namespace game {
    namespace systems {
        Render::Render(Context &context) : systems::BaseSystem(context) {}

        void Render::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
            if (renderer == nullptr) return;

            processTextures();
            processMeshes();

            renderer::FrameContext frameContext = renderer->createFrameContext(es);

            entityx::ComponentHandle<components::Camera> camera;
            entityx::ComponentHandle<components::Transform> cameraTransform;
            entityx::ComponentHandle<components::Target> cameraTarget;

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
            event_manager.subscribe<events::RenderUpdateMesh>(*this);
            event_manager.subscribe<events::RenderResize>(*this);
            event_manager.subscribe<events::ObjectCreate>(*this);
            event_manager.subscribe<ex::EntityDestroyedEvent>(*this);
        }

        void Render::setRenderer(std::shared_ptr<renderer::Renderer> &newRenderer) {
            renderer = newRenderer.get();

            // @todo: handle resize
            const renderer::Params &params = renderer->getParams();
//            context->windowHeight = static_cast<float>(params.getHeight());
//            context->windowWidth = static_cast<float>(params.getWidth());
        }

        void Render::receive(const events::RenderResize &event) {
//            setSize(static_cast<float>(event.width), static_cast<float>(event.height));
        }

        void setSize(float width, float height) {
//            context->windowWidth = width;
//            context->windowHeight = height;
        }

        void Render::receive(const events::RenderUpdateMesh &event) {
            ex::Entity entity = event.entity;
            auto meshComponent = components::get<components::Mesh>(entity);

            if (meshComponent) {
                for (auto &mesh : meshComponent->getAll()) {
                    updateMesh(mesh);
                }
            }
        }

        void Render::receive(const ex::EntityDestroyedEvent &event) {
            ex::Entity entity = event.entity;

            auto meshes = components::get<c::Mesh>(entity);

            if (meshes) {
                for (auto &mesh : meshes->getAll()) {
                    removeMesh(mesh);
                }
                entity.remove<c::Renderable>();
            }
        }

        void Render::receive(const events::ObjectCreate &event) {
            if (event.entity.has_component<components::Renderable>()) {
                ex::Entity entity = event.entity;

                auto meshes = components::get<components::Mesh>(entity);

                if (meshes) {
                    for (auto &mesh : meshes->getAll()) {
                        if (mesh->geometry.renderHandle == nullptr) {
                            addMesh(mesh);
                        }
                    }
                }
            }
        }

        void Render::addMesh(std::shared_ptr<Mesh> &mesh) {
            queueCreateMesh.push(mesh);
        }

        void Render::updateMesh(std::shared_ptr<Mesh> &mesh) {
            queueUpdateMesh.push(mesh);
        }

        void Render::removeMesh(std::shared_ptr<Mesh> &mesh) {
            queueDestroyMesh.push(mesh);
        }

        void Render::addShader(assets::Shader *asset) {
            setupShaders.push(asset);
        }

        void Render::addTexture(assets::Image *asset) {
            queueCreateTexture.push(asset);
        }

        void Render::removeTexture(assets::Image *asset) {
            queueDestroyTexture.push(asset);
        }

        void Render::addMaterial(assets::Material *asset) {
            setupMaterials.push(asset);
        }

        void Render::processMeshes() {
            processQueue<MeshHandle>(queueCreateMesh, [this](MeshHandle &mesh) {
                renderer->createMesh(mesh);
            });

            processQueue<MeshHandle>(queueUpdateMesh, [this](MeshHandle &mesh) {
                renderer->updateMesh(mesh);
            });

            processQueue<MeshHandle>(queueDestroyMesh, [this](MeshHandle &mesh) {
                renderer->destroyMesh(mesh);
            });
        }

        void Render::processTextures() {
            processQueue<assets::Image *>(queueCreateTexture, [this](assets::Image *&asset) {
//                renderer->createTexture(asset->getImage());
            });

            processQueue<assets::Image *>(queueDestroyTexture, [this](assets::Image *&asset) {
//                renderer->destroyTexture();
            });
        }

        void Render::processUi() {

        }
    }
}