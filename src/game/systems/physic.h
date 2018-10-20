//
// Created by demitriy on 3/24/18.
//

#ifndef ACTIO_SYSTEMS_PHYSIC_H
#define ACTIO_SYSTEMS_PHYSIC_H

#include <entityx/entityx/entityx.h>
#include <stack>
#include <entityx/entityx/Event.h>
#include <entityx/entityx/config.h>
#include <entityx/entityx/System.h>
#include "PxPhysics.h"
#include "PxScene.h"
#include "PxRigidDynamic.h"
#include "PxShape.h"
#include "PxPhysicsAPI.h"
#include "cooking/PxCooking.h"
#include "../components/physic.h"
#include "../components/transform.h"
#include "../components/controlled.h"
#include "../components/model.h"
#include "../components/meshes.h"
#include "../events/physic_create.h"
#include "../events/physic_force.h"
#include "../events/setup_controlled.h"
#include "../events/key_press.h"
#include "../events/render_setup_model.h"
#include "../events/physic_contact.h"
#include "../components/base.h"
#include "../components/renderable.h"
#include "../components/character.h"
#include "../components/user_control.h"
#include "../components/physic_entity.h"
#include "../../core/mesh.h"
#include "../../core/geometry.h"
#include "../../core/vertex.h"
#include "../../lib/console.h"
#include "../../lib/image_data.h"
#include "../context.h"
#include "../world_object.h"
#include "base.h"

#define PX_REST_VEC(vec) px::PxVec3(vec.x, vec.y, vec.z)

namespace game {
    class World;

    struct HeightMap {
        explicit HeightMap(const physx::PxU32 &cols, const physx::PxU32 &rows)
            : cols(cols)
            , rows(rows)
            , size(cols * rows)
            , values(new physx::PxI16[cols * rows])
        {}

        ~HeightMap() {
            delete []values;
        }

        physx::PxI16 *values = nullptr;
        physx::PxU32 cols;
        physx::PxU32 rows;
        size_t size;
    };

    namespace systems {
        namespace ex = entityx;
        namespace px = physx;
        namespace c = components;

        static px::PxFilterFlags PhysicFilterShader(
                px::PxFilterObjectAttributes attributes0, px::PxFilterData filterData0,
                px::PxFilterObjectAttributes attributes1, px::PxFilterData filterData1,
                px::PxPairFlags &pairFlags, const void *constantBlock, physx::PxU32 constantBlockSize) {
//            console::info("PhysicFilterShader");
            // let triggers through
            if (px::PxFilterObjectIsTrigger(attributes0) || px::PxFilterObjectIsTrigger(attributes1)) {
//                pairFlags = px::PxPairFlag::eTRIGGER_DEFAULT;
//                return px::PxFilterFlag::eDEFAULT;
            }
//             generate contacts for all that were not filtered above
//            pairFlags = px::PxPairFlag::eCONTACT_DEFAULT;

//            // trigger the contact callback for pairs (A,B) where
//            // the filtermask of A contains the ID of B and vice versa.
//            if((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
//                pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
//
//            pairFlags |= px::PxPairFlag::eCONTACT_DEFAULT | px::PxPairFlag::eNOTIFY_TOUCH_FOUND | px::PxPairFlag::eNOTIFY_TOUCH_LOST;
//            pairFlags = px::PxPairFlag::eCONTACT_DEFAULT;

            pairFlags |= px::PxPairFlag::eCONTACT_DEFAULT | px::PxPairFlag::eNOTIFY_TOUCH_FOUND |
                         px::PxPairFlag::eNOTIFY_TOUCH_LOST;

            return px::PxFilterFlags();
        }

        class Physic
                : systems::BaseSystem
                  , public entityx::System<Physic>
                  , public ex::Receiver<Physic>
                  , public px::PxSimulationEventCallback {
        public:
            enum SceneParam {
                Scene_Gravity
            };

            enum class GeometryType {
                Box,
                Capsule,
                Sphere,
                TriangleMesh
            };

            explicit Physic(World *world);

            ~Physic() override;

            void configure(ex::EntityManager &entities, entityx::EventManager &event_manager) override;

            void postConfigure(entityx::EventManager &events);

            void destroy();

            void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;

            void updateObjects1(ex::EntityManager &es);

            void updateObjects2();

            void setDrawDebug(bool value);

            void drawDebugBuffer(ex::EventManager &events);

            void setSceneGravity(vec3 value);

            px::PxRigidDynamic *createDynamic(const px::PxTransform &t, const px::PxGeometry &geometry, const px::PxVec3 &velocity);

            void spawn(game::WorldObject *object);

            void remove(game::WorldObject *object);

            void makeStatic(game::WorldObject *object);

            void makeDynamic(game::WorldObject *object, const game::WorldObject::Description& description);

            void wave(vec3 position, vec3 direction);

            void computeBoundingBox(game::WorldObject *object);

            void receive(const ex::EntityCreatedEvent &event);

            void receive(const ex::EntityDestroyedEvent &event);

            void receive(const events::PhysicCreateSphere &event);

            void receive(const events::PhysicCreateBox &event);

            void receive(const events::PhysicForce &event);

            void receive(const events::MakeControlled &event);

            void receive(const events::MakeUnControlled &event);

            void receive(const events::KeyPress &event);

            HeightMap *createHeightMap(const std::shared_ptr<ImageData> &image);

            px::PxRigidStatic *generateTerrain(const HeightMap &heightmap, const px::PxReal &width, const px::PxReal &height);

            std::shared_ptr<Mesh> generateTerrainMesh(px::PxRigidStatic *actor, const HeightMap &heightmap);

            px::PxMaterial *findMaterial(const std::string &name);

            void onContact(const px::PxContactPairHeader &pairHeader, const px::PxContactPair *pairs, px::PxU32 nbPairs) override;

            void onTrigger(px::PxTriggerPair *pairs, px::PxU32 count) override;

            void onConstraintBreak(px::PxConstraintInfo *, px::PxU32) override;

            void onWake(px::PxActor **, px::PxU32) override;

            void onSleep(px::PxActor **, px::PxU32) override;

            void onAdvance(const px::PxRigidBody *const *, const px::PxTransform *, const px::PxU32) override;

        private:
            px::PxDefaultAllocator gAllocator;
            px::PxDefaultErrorCallback gErrorCallback;
            px::PxFoundation *gFoundation = nullptr;
            px::PxPhysics *gPhysics = nullptr;
            px::PxPvd *gPvd = nullptr;
            px::PxDefaultCpuDispatcher *cpuDispatcher = nullptr;
            px::PxGpuDispatcher *gpuDispatcher = nullptr;
//            PxCudaContextManager* cudaContextManager = nullptr;
            px::PxScene *gScene = nullptr;
            px::PxControllerManager *gControllerManager = nullptr;
            px::PxCooking *cooking = nullptr;
            std::unordered_map<std::string, px::PxMaterial *> pxMaterials;

            px::PxVec3 controlDir;
            game::WorldObject *debugLinesObject;
            game::WorldObject *debugTrianglesObject;
            game::WorldObject *debugPointsObject;

            std::stack<std::pair<px::PxActor *, px::PxActor *>> contactedActors;

            bool drawDebug = true;
            bool isFirstFrame = true;
        };
    }
}

#endif //ACTIO_SYSTEMS_PHYSIC_H
