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

#include <entityx/entityx.h>
#include <string>
#include "LuaBridge/LuaBridge.h"
#include "components/base.h"
#include "components/transform.h"
#include "desc/light_directional.h"
#include "desc/light_point.h"
#include "desc/light_spot.h"
#include "../core/material.h"
#include "../lib/assets_types.h"
#include "../lib/console.h"
#include "../core/geometry_primitive.h"
#include "../core/mesh.h"
#include "world_object.h"
#include "context.h"


namespace game {
    namespace ex = entityx;
    namespace c = components;

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

        void evalUpdate(const float &dt);

        const std::string getName();

        bool isInited();

        struct WorldLib {
            explicit WorldLib(World *world) : world(world) {}

            void addDirectionalLight(desc::LightDirectionalDesc &description);

            void addPointLight(desc::LightPointDesc &description);

            void addSpotLight(desc::LightSpotDesc &description);

            Mesh *createMesh();

            Mesh *createMesh(const std::string &materialName);

            ex::Entity createModelEntity(Mesh *mesh);

            WorldObject createObject(ex::Entity &entity);

            void spawnObject(WorldObject *object);

            void spawnObject(WorldObject *object, const vec3 &position);

            Material createMaterial();

            void findMaterial(const std::string &name);

            game::WorldObject *createStaticObject();

            game::WorldObject *createDynamicObject();

            game::WorldObject *createStaticObject(const vec3 &position);

            game::WorldObject *createDynamicObject(const vec3 &position);

            World *world = nullptr;
        };

        struct Math1Lib {
            static float clamp(const float &v, const float &a, const float &b);

            static float sin(const float &v);

            static float cos(const float &v);

            static float abs(const float &v);

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

        struct GeometryLib {
            static void createBox(Mesh *mesh, float width, float height);

            static void createPlane(Mesh *mesh, uint width, uint height);

            static void createSphere(Mesh *mesh, float radius, uint widthSegments, uint heightSegments);

            static void createCircle(Mesh *mesh, float radius, uint segments);

            static void createCone(Mesh *mesh, float radius, float height, uint radialSegments, uint heightSegments);

            static void
            createCylinder(Mesh *mesh, float radiusTop, float radiusBottom, float height, uint radialSegments,
                           uint heightSegments);

            static void
            createRing(Mesh *mesh, float innerRadius, float outerRadius, uint thetaSegments, uint phiSegments);

            static void
            createTorus(Mesh *mesh, float radius, float tube, uint radialSegments, uint tubularSegments, float arc);

            static void createOctahedron(Mesh *mesh, float radius);

            static void createQuad2d(Mesh *mesh);

            static void createLines(Mesh *mesh, std::vector<vec3> &lines);
        };

        struct EntityLib {

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
