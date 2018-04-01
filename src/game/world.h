#ifndef GAME_WORLD_H
#define GAME_WORLD_H

#define NDEBUG

#include <string>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include "entityx/entityx.h"
#include "character.h"
#include "../lib/console.h"
#include "../lib/input_handler.h"
#include "../resources/file_resource.h"
#include "../lib/ini_loader.h"
#include "../renderer/base_renderer.h"

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

        void setupMovement(InputHandler *ih);

        void setup();

        void initFromFile(const Resource::File &file);

        void initGroudPlane();

        void destroy();

        inline void spawn(Character *character);

        void spawn(Character *character, glm::vec3 pos);

        void add();

        void remove();

        void update(TimeDelta dt);

        void render(TimeDelta dt);

    private:
        std::string name;
        uint time;

        std::vector<WorldObject<Character>> characters;
    };

    struct WorldController {
        static void move(World *world, Character *character);
    };
}

#endif
