#ifndef GAME_SYSTEMS_RENDER_H_
#define GAME_SYSTEMS_RENDER_H_

#include <unordered_map>
#include <vector>
#include <chrono>
#include <stack>
#include <memory>
#include "entityx/entityx.h"
#include "../components/base.h"
#include "../components/renderable.h"
#include "../components/transform.h"
#include "../components/model.h"
#include "../events/object_create.h"
#include "../events/render_resize.h"
#include "../events/render_update_mesh.h"
#include "../../lib/console.h"
#include "../../lib/assets_shader.h"
#include "../../lib/assets_texture.h"
#include "../../lib/assets_material.h"
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
    class World;

    namespace systems {
        namespace ex = entityx;
        namespace c = components;

        class Render
                : public systems::BaseSystem
                  , public entityx::System<Render>
                  , public ex::Receiver<Render> {
        private:
            enum class ModelAction {
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
            explicit Render(Context& context);

            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;

            void configure(entityx::EventManager &event_manager) override;

            void setRenderer(renderer::Renderer *newRenderer);

            void receive(const events::RenderResize &event);

            void receive(const events::RenderUpdateMesh &event);

            void receive(const events::ObjectCreate &event);

            void receive(const ex::EntityDestroyedEvent &event);

            void setSize(float width, float height);

            void addMesh(std::shared_ptr<Mesh> &mesh);

            void updateMesh(std::shared_ptr<Mesh> &mesh);

            void removeMesh(std::shared_ptr<Mesh> &mesh);

            void addShader(assets::Shader *asset);

            void addTexture(assets::Texture *asset);

            void removeTexture(assets::Texture *asset);

            void addMaterial(assets::Material *asset);

        private:
            void processTextures();

            void processMeshes();

            void processUi();

            renderer::Renderer *renderer = nullptr;

            std::stack<MeshHandle> queueCreateMesh;
            std::stack<MeshHandle> queueUpdateMesh;
            std::stack<MeshHandle> queueDestroyMesh;

            std::stack<assets::Texture*> queueCreateTexture;
            std::stack<assets::Texture*> queueDestroyTexture;

            std::stack<assets::Shader*> setupShaders;
            std::stack<assets::Material*> setupMaterials;
        };
    }
}

#endif