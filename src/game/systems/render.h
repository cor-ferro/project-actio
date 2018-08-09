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
    class World;

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
            explicit Render(World *world, renderer::Renderer *newRenderer);

            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;

            void configure(entityx::EventManager &event_manager) override;

            void receive(const events::RenderResize &event);

            void receive(const events::RenderSetupModel &event);

            void receive(const ex::EntityDestroyedEvent &event);

            void addModel(Model *model);

            void updateModel(Model *model);

            void removeModel(Model *model);

            void addMesh(Mesh *mesh);

            void updateMesh(Mesh *mesh);

            void removeMesh(Mesh *mesh);

            void addShader(assets::Shader *asset);

            void addTexture(assets::Texture *asset);

            void removeTexture(assets::Texture *asset);

            void addMaterial(assets::Material *asset);

            void destroy();

        private:
            void processShaders();

            void processTextures();

            void processMeshes();

            int updatePerTick = 30;

            renderer::Renderer *renderer = nullptr;

            std::stack<std::tuple<MeshAction, Mesh *>> setupMesh;
            std::stack<assets::Shader*> setupShaders;
            std::stack<std::pair<assets::Texture*, TextureAction>> setupTextures;
            std::stack<assets::Material*> setupMaterials;
        };
    }
}

#endif