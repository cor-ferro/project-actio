#include <glm/glm.hpp>
#include "script.h"
#include "../core/geometry_primitive.h"

namespace game {
    struct ScriptApi {
        void static log(const std::string &s) {
            console::info("%s", s);
        }
    };


    /* ----- Script math layer ----- */

    float ScriptMathApi::clamp(const float &v, const float &a, const float &b) {
        return glm::clamp(v, a, b);
    }


    /* ----- Script math3 layer ----- */

    glm::vec3 ScriptMath3Api::clamp(const glm::vec3 &vec, const float &a, const float &b) {
        return glm::clamp(vec, a, b);
    }

    glm::vec3 ScriptMath3Api::normalize(const glm::vec3 &vec) {
        return glm::normalize(vec);
    }

    glm::vec3 ScriptMath3Api::cross(const glm::vec3 &vec1, const glm::vec3 &vec2) {
        return glm::cross(vec1, vec2);
    }

    float ScriptMath3Api::distance(const glm::vec3 &vec1, const glm::vec3 &vec2) {
        return glm::distance(vec1, vec2);
    }

    float ScriptMath3Api::dot(const glm::vec3 &vec1, const glm::vec3 &vec2) {
        return glm::dot(vec1, vec2);
    }

    float ScriptMath3Api::length(const glm::vec3 &vec1) {
        return glm::length(vec1);
    }

    glm::vec3 ScriptMath3Api::reflect(const glm::vec3 &vec1, const glm::vec3 &vec2) {
        return glm::reflect(vec1, vec2);
    }

    glm::vec3 ScriptMath3Api::refract(const glm::vec3 &vec1, const glm::vec3 &vec2) {
        return glm::reflect(vec1, vec2);
    }

    glm::vec3 ScriptMath3Api::mul(const glm::vec3 &vec1, const glm::vec3 &vec2) {
        return vec1 * vec2;
    }

    glm::vec3 ScriptMath3Api::div(const glm::vec3 &vec1, const glm::vec3 &vec2) {
        return vec1 / vec2;
    }

    glm::vec3 ScriptMath3Api::add(const glm::vec3 &vec1, const glm::vec3 &vec2) {
        return vec1 + vec2;
    }

    glm::vec3 ScriptMath3Api::sub(const glm::vec3 &vec1, const glm::vec3 &vec2) {
        return vec1 - vec2;
    }

    struct ScriptGeometryApi {
        void static Box() {}
        void static Circle() {}
    };

    struct Vec2Helper {
        template<unsigned index>
        static float get(glm::vec2 const *vec) {
            return vec->operator[](index);
        }

        template<unsigned index>
        static void set(glm::vec2 *vec, float value) {
            vec->operator[](index) = value;
        }
    };

    struct Vec3Helper {
        template<unsigned index>
        static float get(glm::vec3 const *vec) {
            return vec->operator[](index);
        }

        template<unsigned index>
        static void set(glm::vec3 *vec, float value) {
            vec->operator[](index) = value;
        }
    };


    Script::Script(game::assets::Script *asset) : asset(asset) {

    }

    Script::~Script() {
        if (L != nullptr) {
            lua_close(L);
        }
    }

    void Script::init(Context &context) {
        try {
            if (asset == nullptr) {
                console::warn("Script %s, asset is empty, nothing to init", getName());
                return;
            }

            inited = false;
            L = luaL_newstate();

            const char *source = asset->getContent();

            if (source == nullptr) {
                console::warn("Script %s source is empty", getName());
                return;
            }

            luaL_dostring(L, source);
            luaL_openlibs(L);

            luabridge::getGlobalNamespace(L)
                    .addFunction("log", &ScriptApi::log);

            luabridge::getGlobalNamespace(L)
                    .beginClass<glm::vec2>("vec2")
                        .addConstructor<void (*) (float)>()
                        .addConstructor<void (*) (float, float)>()
                        .addProperty("x", &Vec2Helper::get<0>, &Vec2Helper::set<0>)
                        .addProperty("y", &Vec2Helper::get<1>, &Vec2Helper::set<1>)
                    .endClass();

            luabridge::getGlobalNamespace(L)
                    .beginClass<glm::vec3>("vec3")
                        .addConstructor<void (*) (float)>()
                        .addConstructor<void (*) (float, float, float)>()
                        .addProperty("x", &Vec3Helper::get<0>, &Vec3Helper::set<0>)
                        .addProperty("y", &Vec3Helper::get<1>, &Vec3Helper::set<1>)
                        .addProperty("z", &Vec3Helper::get<2>, &Vec3Helper::set<2>)
                    .endClass();

            luabridge::getGlobalNamespace(L)
                    .beginNamespace("context")
                        .addVariable("mousePosition", &context.mousePosition, false)
                        .addVariable("mouseWorldPosition", &context.mouseWorldPosition, false)
                        .addVariable("screenWidth", &context.screenWidth, false)
                        .addVariable("screenHeight", &context.screenHeight, false)
                        .addVariable("windowWidth", &context.windowWidth, false)
                        .addVariable("windowHeight", &context.windowHeight, false)
                    .endNamespace();

            luabridge::getGlobalNamespace(L)
                    .beginNamespace("math")
                        .addFunction("clamp", &ScriptMathApi::clamp)
                    .endNamespace();

            luabridge::getGlobalNamespace(L)
                    .beginNamespace("math3")
                        .addFunction("clamp", &ScriptMath3Api::clamp)
                        .addFunction("normalize", &ScriptMath3Api::normalize)
                        .addFunction("cross", &ScriptMath3Api::cross)
                        .addFunction("distance", &ScriptMath3Api::distance)
                        .addFunction("dot", &ScriptMath3Api::dot)
                        .addFunction("length", &ScriptMath3Api::length)
                        .addFunction("reflect", &ScriptMath3Api::reflect)
                        .addFunction("refract", &ScriptMath3Api::refract)
                        .addFunction("mul", &ScriptMath3Api::mul)
                        .addFunction("div", &ScriptMath3Api::div)
                        .addFunction("add", &ScriptMath3Api::add)
                        .addFunction("sub", &ScriptMath3Api::sub)
                    .endNamespace();

            luabridge::getGlobalNamespace(L)
                    .beginNamespace("geometry")
//                            .addFunction("Box", GeometryPrimitive::Box)
//                            .addFunction("Circle", GeometryPrimitive::Sphere)
                    .endNamespace();

            luabridge::LuaRef fnStart = luabridge::getGlobal(L, "start");
            if (fnStart.isNil()) {
                console::warn("function start is not defined");
                return;
            }

            luabridge::LuaRef fnUpdate = luabridge::getGlobal(L, "update");
            if (fnUpdate.isNil()) {
                console::warn("function update is not defined");
                return;
            }

            inited = true;
        } catch (luabridge::LuaException &e) {
            console::err("failed init lua script: %s", e.what());
        } catch (...) {
            console::err("failed init lua script: unknown error");
        }
    }

    void Script::evalStart() {
        if (!inited) return;

        luabridge::LuaRef fn = luabridge::getGlobal(L, "start");

        fn();
    }

    void Script::evalUpdate() {
        if (!inited) return;

        luabridge::LuaRef fn = luabridge::getGlobal(L, "update");

        fn();
    }

    const std::string Script::getName() {
        return name;
    }
}