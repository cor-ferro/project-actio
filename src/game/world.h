#ifndef GAME_WORLD_H
#define GAME_WORLD_H

#define NDEBUG

#include <string>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include "entityx/entityx.h"
#include "PxPhysics.h"
#include "PxScene.h"
#include "PxRigidDynamic.h"
#include "PxShape.h"
#include "PxPhysicsAPI.h"
#include "pvd/PxPvd.h"
#include "character.h"
#include "../lib/console.h"
#include "../resources/file_resource.h"
#include "../lib/ini_loader.h"

namespace game {
    using namespace physx;
    using namespace entityx;

    typedef glm::vec3 vec3;
    typedef glm::vec2 vec2;
    typedef glm::mat4 mat4;

    struct World : EntityX {
        template <typename Object>
        struct WorldObject {
            Object* object;
            vec3 position;
            bool frozen;
            bool hidden;
        };

        World();

        void initFromFile(const Resource::File& file);
        void destroy();

        inline void spawn(Character* character);
        void spawn(Character* character, glm::vec3 pos);
        void add();
        void remove();

        void update(TimeDelta dt);
        void render(TimeDelta dt);
        void subscribe();
        void unsubscribe();

        void updatePhysics();

        void initPhysics();
        void stepPhysics();
        void applyPhysics();
        void cleanupPhysics();

    private:
        std::string name;
        uint time;

        std::vector<WorldObject<Character>> characters;

        PxDefaultAllocator gAllocator;
        PxDefaultErrorCallback gErrorCallback;
        PxFoundation* gFoundation = NULL;
        PxPhysics*  gPhysics = NULL;
        PxPvd* gPvd = NULL;
        PxDefaultCpuDispatcher* gDispatcher = NULL;
        PxScene* gScene = NULL;
        std::unordered_map<std::string, PxMaterial*> pxMaterials;

        PxRigidDynamic* createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity=PxVec3(0));
    };

    struct WorldController {
        static void move(World* world, Character* character);
    };
}

#endif