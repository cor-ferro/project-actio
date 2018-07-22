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
#include "assets_types.h"
#include "../lib/console.h"
#include "context.h"

namespace game {
    struct ScriptMathApi {
        float static clamp(const float &v, const float &a, const float &b);
    };

    struct ScriptMath3Api {
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

    class Script {
    public:
        explicit Script(assets::Script *asset);

        ~Script();

        void init(Context &context);

        void evalStart();

        void evalUpdate();

        const std::string getName();

    private:
        std::string name;
        assets::Script *asset;
        lua_State *L = nullptr;
        bool inited = false;
    };
}

#endif //ACTIO_GAME_SCRIPT_H
