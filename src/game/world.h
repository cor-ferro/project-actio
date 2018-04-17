#ifndef GAME_WORLD_H
#define GAME_WORLD_H

#define NDEBUG

#include <string>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include "entityx/entityx.h"
#include "character.h"
#include "components/skin.h"
#include "../lib/console.h"
#include "../lib/input_handler.h"
#include "../resources/file_resource.h"
#include "../lib/ini_loader.h"
#include "../renderer/base_renderer.h"
#include "systems/physic.h"
#include "systems/camera.h"
#include "desc/light_point.h"
#include "desc/light_directional.h"
#include "desc/light_spot.h"

namespace game {
    using namespace entityx;

    typedef glm::vec3 vec3;
    typedef glm::vec2 vec2;
    typedef glm::mat4 mat4;

    struct World : EntityX {
        template<typename Object>
        struct WorldObject {
            Object *object;
            vec3 position;
            bool frozen;
            bool hidden;
        };

        World();

        void setupRenderer(renderer::Renderer *);

        void setupInput(InputHandler *ih);

        void setup();

        void initGroudPlane();

        void destroy();

        inline void spawn(Character *character);

        void spawn(Character *character, glm::vec3 pos);

        void add();

        void remove();

        void addLight(desc::LightPointDesc);

        void addLight(desc::LightDirectionalDesc);

        void addLight(desc::LightSpotDesc);

        void removeLight(entityx::Entity entity);

        void update(TimeDelta dt);

        void render(TimeDelta dt);

        void setPhysicsDebug(bool value);

        void setLightDebug(bool value);

        void setCameraSettings(float fov, float aspect, float near, float far);

        void setGravity(vec3 dir);

        void cameraLookAt(vec3 target);

    private:
        std::string name;
        double time;

        std::vector<WorldObject<Character>> characters;

        std::shared_ptr<game::systems::Physic> physic = nullptr;
        std::shared_ptr<game::systems::Camera> camera = nullptr;
    };

    struct WorldController {
        static void move(World *world, Character *character);
    };
}

#endif
