#include <glm/glm.hpp>
#include "script.h"
#include "world.h"
#include "../lib/assets_resource.h"
#include "../lib/assets_types.h"
#include "../core/geometry_primitive.h"

namespace game {
    struct ScriptApi {
        void static log(const std::string &s) {
            console::info("%s", s);
        }
    };

    void Script::WorldLib::addDirectionalLight(desc::LightDirectionalDesc &description) {
        world->addLight(description);
    };

    void Script::WorldLib::addPointLight(game::desc::LightPointDesc &description) {
        world->addLight(description);
    }

    void Script::WorldLib::addSpotLight(game::desc::LightSpotDesc &description) {
        world->addLight(description);
    }

    void Script::WorldLib::addGeometry() {

    }


    /* ----- Script math layer ----- */

    float Script::Math1Lib::clamp(const float &v, const float &a, const float &b) {
        return glm::clamp(v, a, b);
    }

    float Script::Math1Lib::PI = glm::pi<float>();
    float Script::Math1Lib::TWO_PI = glm::two_pi<float>();
    float Script::Math1Lib::HALF_PI = glm::half_pi<float>();
    float Script::Math1Lib::EPSILON = glm::epsilon<float>();
    float Script::Math1Lib::E = glm::e<float>();

    /* ----- Script math3 layer ----- */

    glm::vec3 Script::Math3Lib::clamp(const glm::vec3 &vec, const float &a, const float &b) {
        return glm::clamp(vec, a, b);
    }

    glm::vec3 Script::Math3Lib::normalize(const glm::vec3 &vec) {
        return glm::normalize(vec);
    }

    glm::vec3 Script::Math3Lib::cross(const glm::vec3 &vec1, const glm::vec3 &vec2) {
        return glm::cross(vec1, vec2);
    }

    float Script::Math3Lib::distance(const glm::vec3 &vec1, const glm::vec3 &vec2) {
        return glm::distance(vec1, vec2);
    }

    float Script::Math3Lib::dot(const glm::vec3 &vec1, const glm::vec3 &vec2) {
        return glm::dot(vec1, vec2);
    }

    float Script::Math3Lib::length(const glm::vec3 &vec1) {
        return glm::length(vec1);
    }

    glm::vec3 Script::Math3Lib::reflect(const glm::vec3 &vec1, const glm::vec3 &vec2) {
        return glm::reflect(vec1, vec2);
    }

    glm::vec3 Script::Math3Lib::refract(const glm::vec3 &vec1, const glm::vec3 &vec2) {
        return glm::reflect(vec1, vec2);
    }

    glm::vec3 Script::Math3Lib::mul(const glm::vec3 &vec1, const glm::vec3 &vec2) {
        return vec1 * vec2;
    }

    glm::vec3 Script::Math3Lib::div(const glm::vec3 &vec1, const glm::vec3 &vec2) {
        return vec1 / vec2;
    }

    glm::vec3 Script::Math3Lib::add(const glm::vec3 &vec1, const glm::vec3 &vec2) {
        return vec1 + vec2;
    }

    glm::vec3 Script::Math3Lib::sub(const glm::vec3 &vec1, const glm::vec3 &vec2) {
        return vec1 - vec2;
    }

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


    Script::Script(assets::Script *asset) : asset(asset) {
        console::info("create script");
    }

    Script::~Script() {
        destroyLuaState();

        delete worldApi;
    }

    Script::INIT_STATUS Script::init(World *world) {
        INIT_STATUS status = INIT_STATUS::SUCCESS;

        try {
            inited = false;

            worldApi = new Script::WorldLib(world);

            int createStatus = createLuaState();
            if (createStatus == 0) {
                link();
                inited = true;
            } else {
                console::warn("failed create lua state, %i", createStatus);
                status = INIT_STATUS::LUA_STATE_ERROR;
            }

        } catch (luabridge::LuaException &e) {
            console::err("failed init lua script: %s", e.what());
            status = INIT_STATUS::LUA_EXCEPTION;
        } catch (...) {
            console::err("failed init lua script: unknown error");
            status = INIT_STATUS::ERROR;
        }

        return status;
    }

    void Script::evalStart() {
        if (!inited) return;

        luabridge::LuaRef fn = luabridge::getGlobal(L, "start");

        fn();
    }

    void Script::evalUpdate() {
        if (!inited) return;
        if (isFirstCall) {
            evalStart();
            isFirstCall = false;
        }

        luabridge::LuaRef fn = luabridge::getGlobal(L, "update");

        fn();
    }

    const std::string Script::getName() {
        return name;
    }

    int Script::createLuaState() {
        assert(L == nullptr);

        if (asset == nullptr) {
            console::warn("script %s, asset is empty, nothing to init", getName());
            return 1;
        }

        assets::Resource::Content source = asset->getContent();

        if (source == nullptr) {
            console::warn("Script %s source is empty", getName());
            return 1;
        }

        L = luaL_newstate();

        int status = luaL_dostring(L, source->c_str());

        if (status != 0)
        {
            console::err("Lua script err: %s", lua_tostring(L, -1));
            lua_pop(L, 1);

            return 1;
        }

        return 0;
    }

    int Script::destroyLuaState() {
        if (L != nullptr) {
            lua_close(L);
            L = nullptr;

            return 0;
        }

        return 1;
    }

    void Script::link() {
//            luaL_openlibs(L); // open all libs
        luaopen_base(L);
        luaopen_table(L);
        luaopen_string(L);
        luaopen_math(L);
        luaopen_package(L);
        luaopen_debug(L);

        luabridge::getGlobalNamespace(L)
            .addFunction("log", &ScriptApi::log);

        luabridge::getGlobalNamespace(L)
            .beginClass<glm::vec2>("vec2")
                .addConstructor<void (*) (float)>()
                .addConstructor<void (*) (float, float)>()
                .addProperty("x", &Vec2Helper::get<0>, &Vec2Helper::set<0>)
                .addProperty("y", &Vec2Helper::get<1>, &Vec2Helper::set<1>)
            .endClass()
        ;

        luabridge::getGlobalNamespace(L)
            .beginClass<glm::vec3>("vec3")
                .addConstructor<void (*) (float)>()
                .addConstructor<void (*) (float, float, float)>()
                .addProperty("x", &Vec3Helper::get<0>, &Vec3Helper::set<0>)
                .addProperty("y", &Vec3Helper::get<1>, &Vec3Helper::set<1>)
                .addProperty("z", &Vec3Helper::get<2>, &Vec3Helper::set<2>)
            .endClass()
        ;

        luabridge::getGlobalNamespace(L)
            .beginNamespace("light")
                .beginClass<desc::LightDirectionalDesc>("Directional")
                    .addConstructor<void (*) ()>()
                    .addProperty("ambient", &desc::LightDirectionalDesc::getAmbient, &desc::LightDirectionalDesc::setAmbient)
                    .addProperty("diffuse", &desc::LightDirectionalDesc::getDiffuse, &desc::LightDirectionalDesc::setDiffuse)
                    .addProperty("specular", &desc::LightDirectionalDesc::getSpecular, &desc::LightDirectionalDesc::setSpecular)
                    .addProperty("position", &desc::LightDirectionalDesc::getPosition, &desc::LightDirectionalDesc::setPosition)
                    .addProperty("direction", &desc::LightDirectionalDesc::getDirection, &desc::LightDirectionalDesc::setDirection)
                .endClass()
                .beginClass<desc::LightPointDesc>("Point")
                .addConstructor<void (*) ()>()
                    .addProperty("ambient", &desc::LightPointDesc::getAmbient, &desc::LightPointDesc::setAmbient)
                    .addProperty("diffuse", &desc::LightPointDesc::getDiffuse, &desc::LightPointDesc::setDiffuse)
                    .addProperty("specular", &desc::LightPointDesc::getSpecular, &desc::LightPointDesc::setSpecular)
                    .addProperty("position", &desc::LightPointDesc::getPosition, &desc::LightPointDesc::setPosition)
                    .addProperty("direction", &desc::LightPointDesc::getDirection, &desc::LightPointDesc::setDirection)
                    .addProperty("constant", &desc::LightPointDesc::getConstant, &desc::LightPointDesc::setConstant)
                    .addProperty("linear", &desc::LightPointDesc::getLinear, &desc::LightPointDesc::setLinear)
                    .addProperty("quadratic", &desc::LightPointDesc::getQuadratic, &desc::LightPointDesc::setQuadratic)
                .endClass()
                .beginClass<desc::LightSpotDesc>("Spot")
                    .addConstructor<void (*) ()>()
                    .addProperty("ambient", &desc::LightSpotDesc::getAmbient, &desc::LightSpotDesc::setAmbient)
                    .addProperty("diffuse", &desc::LightSpotDesc::getDiffuse, &desc::LightSpotDesc::setDiffuse)
                    .addProperty("specular", &desc::LightSpotDesc::getSpecular, &desc::LightSpotDesc::setSpecular)
                    .addProperty("position", &desc::LightSpotDesc::getPosition, &desc::LightSpotDesc::setPosition)
                    .addProperty("direction", &desc::LightSpotDesc::getDirection, &desc::LightSpotDesc::setDirection)
                    .addProperty("constant", &desc::LightSpotDesc::getConstant, &desc::LightSpotDesc::setConstant)
                    .addProperty("linear", &desc::LightSpotDesc::getLinear, &desc::LightSpotDesc::setLinear)
                    .addProperty("quadratic", &desc::LightSpotDesc::getQuadratic, &desc::LightSpotDesc::setQuadratic)
                    .addProperty("cutOff", &desc::LightSpotDesc::getCutOff, &desc::LightSpotDesc::setCutOff)
                    .addProperty("outerCutOff", &desc::LightSpotDesc::getOuterCutOff, &desc::LightSpotDesc::setOuterCutOff)
                .endClass()
            .endNamespace();

//            luabridge::getGlobalNamespace(L)
//                    .beginNamespace("geometry")
////                            .addFunction("Box", GeometryPrimitive::Box)
////                            .addFunction("Circle", GeometryPrimitive::Sphere)
//                    .endNamespace();


        const game::Context worldContext = worldApi->world->getContext();

        luabridge::getGlobalNamespace(L)
            .beginNamespace("h")
                .beginNamespace("context")
                    .addVariable("mousePosition", const_cast<vec2*>(&worldContext.mousePosition), false)
                    .addVariable("mouseWorldPosition", const_cast<vec3*>(&worldContext.mouseWorldPosition), false)
                    .addVariable("screenWidth", const_cast<float*>(&worldContext.screenWidth), false)
                    .addVariable("screenHeight", const_cast<float*>(&worldContext.screenHeight), false)
                    .addVariable("windowWidth", const_cast<float*>(&worldContext.windowWidth), false)
                    .addVariable("windowHeight", const_cast<float*>(&worldContext.windowHeight), false)
                .endNamespace()
                .beginClass<Script::WorldLib>("Script::WorldLib")
                    .addFunction("addDirectionalLight", &Script::WorldLib::addDirectionalLight)
                    .addFunction("addPointLight", &Script::WorldLib::addPointLight)
                    .addFunction("addSpotLight", &Script::WorldLib::addSpotLight)
                    .addFunction("addGeometry", &Script::WorldLib::addGeometry)
                .endClass()
                .addVariable("world", worldApi)
            .endNamespace()
        ;

        luabridge::getGlobalNamespace(L)
            .beginNamespace("math1")
                .addFunction("clamp", &Math1Lib::clamp)
                .addVariable("PI", &math.PI, false)
                .addVariable("TWO_PI", &math.TWO_PI, false)
                .addVariable("HALF_PI", &math.HALF_PI, false)
                .addVariable("EPSILON", &math.EPSILON, false)
                .addVariable("E", &math.E, false)
            .endNamespace()
            .beginNamespace("math3")
                .addFunction("clamp", &Script::Math3Lib::clamp)
                .addFunction("normalize", &Script::Math3Lib::normalize)
                .addFunction("cross", &Script::Math3Lib::cross)
                .addFunction("distance", &Script::Math3Lib::distance)
                .addFunction("dot", &Script::Math3Lib::dot)
                .addFunction("length", &Script::Math3Lib::length)
                .addFunction("reflect", &Script::Math3Lib::reflect)
                .addFunction("refract", &Script::Math3Lib::refract)
                .addFunction("mul", &Script::Math3Lib::mul)
                .addFunction("div", &Script::Math3Lib::div)
                .addFunction("add", &Script::Math3Lib::add)
                .addFunction("sub", &Script::Math3Lib::sub)
            .endNamespace()
        ;

        luabridge::LuaRef fnStart = luabridge::getGlobal(L, "start");
        if (fnStart.isNil()) {
            lua_close(L);
            console::warn("function start is not defined");
            return;
        }

        luabridge::LuaRef fnUpdate = luabridge::getGlobal(L, "update");
        if (fnUpdate.isNil()) {
            lua_close(L);
            console::warn("function update is not defined");
            return;
        }
    }

    bool Script::isInited() {
        return inited;
    }
}