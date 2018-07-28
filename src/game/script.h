//
// Created by demitriy on 7/7/18.
//

#ifndef ACTIO_GAME_SCRIPT_H
#define ACTIO_GAME_SCRIPT_H

extern "C" {
# include "lua.h"
# include "lauxlib.h"
# include "lualib.h"
}

#include <string>
#include "LuaBridge/LuaBridge.h"
#include "../lib/assets_types.h"
#include "../lib/console.h"
#include "context.h"
#include "desc/light_directional.h"
#include "desc/light_point.h"
#include "desc/light_spot.h"


namespace game {
    class World;

    class Script {
    public:
        enum class INIT_STATUS {
            SUCCESS = 0,
            LUA_STATE_ERROR = 1,
            LUA_EXCEPTION = 2,
            ERROR = 3
        };

        explicit Script(assets::Script *asset);

        ~Script();

        INIT_STATUS init(World *world);

        void evalStart();

        void evalUpdate();

        const std::string getName();

        bool isInited();

        struct WorldLib {
            explicit WorldLib(World *world) : world(world) {}

            void addDirectionalLight(desc::LightDirectionalDesc &description);

            void addPointLight(desc::LightPointDesc &description);

            void addSpotLight(desc::LightSpotDesc &description);

            void addGeometry();

            World *world = nullptr;
        };

        struct Math1Lib {
            static float clamp(const float &v, const float &a, const float &b);
            static float PI;
            static float TWO_PI;
            static float HALF_PI;
            static float EPSILON;
            static float E;
        };

        struct Math3Lib {
            glm::vec3 static clamp(const glm::vec3 &vec, const float &a, const float &b);
            glm::vec3 static normalize(const glm::vec3 &vec);
            glm::vec3 static cross(const glm::vec3 &vec1, const glm::vec3 &vec2);
            float static distance(const glm::vec3 &vec1, const glm::vec3 &vec2);
            float static dot(const glm::vec3 &vec1, const glm::vec3 &vec2);
            float static length(const glm::vec3 &vec);
            glm::vec3 static reflect(const glm::vec3 &vec1, const glm::vec3 &vec2);
            glm::vec3 static refract(const glm::vec3 &vec1, const glm::vec3 &vec2);
            glm::vec3 static mul(const glm::vec3 &vec1, const glm::vec3 &vec2);
            glm::vec3 static div(const glm::vec3 &vec1, const glm::vec3 &vec2);
            glm::vec3 static add(const glm::vec3 &vec1, const glm::vec3 &vec2);
            glm::vec3 static sub(const glm::vec3 &vec1, const glm::vec3 &vec2);
        };

    private:
        std::string name;
        bool inited = false;
        bool isFirstCall = true;

        assets::Script *asset = nullptr;
        lua_State *L = nullptr;

        WorldLib *worldApi = nullptr;
        Math1Lib math;

        int createLuaState();
        int destroyLuaState();
        void link();
    };
}

#endif //ACTIO_GAME_SCRIPT_H
