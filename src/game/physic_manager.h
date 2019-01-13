//
// Created by demitriy on 11/10/18.
//

#ifndef ACTIO_PHYSIC_MANAGER_H
#define ACTIO_PHYSIC_MANAGER_H

#include <entityx/Entity.h>
#include <unordered_map>
#include <string>

#include "PxPhysics.h"
#include "PxScene.h"
#include "PxRigidDynamic.h"
#include "PxShape.h"
#include "PxPhysicsAPI.h"
#include "cooking/PxCooking.h"

#include "components/physic_actor.h"

#include "../lib/console.h"
#include "../lib/types.h"

namespace game {
    namespace px = physx;
    namespace ex = entityx;

    static px::PxFilterFlags PhysicFilterShader(
            px::PxFilterObjectAttributes attributes0, px::PxFilterData filterData0,
            px::PxFilterObjectAttributes attributes1, px::PxFilterData filterData1,
            px::PxPairFlags& pairFlags, const void *constantBlock, physx::PxU32 constantBlockSize) {
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

    class PhysicManager : public px::PxSimulationEventCallback {
    public:
        PhysicManager();

        ~PhysicManager() override;

        void init();

        void destroy();

        void update(const px::PxReal& dt);

        px::PxPhysics *const getPhysics();

        px::PxScene *const getScene();

        px::PxControllerManager *const getControllerManager();

        px::PxCooking *const getCooking();

        void onContact(const px::PxContactPairHeader& pairHeader, const px::PxContactPair *pairs,
                       px::PxU32 nbPairs) override;

        void onTrigger(px::PxTriggerPair *pairs, px::PxU32 count) override;

        void onConstraintBreak(px::PxConstraintInfo *, px::PxU32) override;

        void onWake(px::PxActor **, px::PxU32) override;

        void onSleep(px::PxActor **, px::PxU32) override;

        void onAdvance(const px::PxRigidBody *const *, const px::PxTransform *, px::PxU32) override;

        void setSceneGravity(const vec3& value);

        const px::PxVec3 getSceneGravity();

        px::PxMaterial *const getMaterial();

        px::PxMaterial *const getMaterial(const std::string& name);

        void wave(const vec3& position, const vec3& direction);

        void applyForce(px::PxRigidDynamic *actor, const vec3& direction);

        void applyForce(ex::Entity entity, const vec3& direction);

        px::PxRigidDynamic *createDynamicActor(const px::PxTransform& transform);

        px::PxRigidDynamic *createDynamicActor(const px::PxVec3& pos);

        px::PxRigidDynamic *createDynamicActor(const vec3& pos);

        px::PxRigidStatic *createStaticActor(const px::PxTransform& transform);

        px::PxRigidStatic *createStaticActor(const px::PxVec3& pos);

        px::PxRigidStatic *createStaticActor(const vec3& pos);

        void addToScene(px::PxRigidActor *actor);

        void removeFromScene(px::PxRigidActor *actor);

        void createPlaneGeometry(px::PxRigidActor *actor);

        void createSphereGeometry(px::PxRigidActor *actor, const px::PxReal& radius);

        void createBoxGeometry(px::PxRigidActor *actor, const px::PxVec3& size);

        void createCapsuleGeometry(px::PxRigidActor *actor, const px::PxReal &radius, const px::PxReal &halfHeight);

    private:
        px::PxDefaultAllocator m_allocator;
        px::PxDefaultErrorCallback m_errorCallback;
        px::PxFoundation *m_foundation = nullptr;
        px::PxPhysics *m_physics = nullptr;
        px::PxPvd *m_pvd = nullptr;
        px::PxDefaultCpuDispatcher *m_cpuDispatcher = nullptr;
        px::PxGpuDispatcher *m_gpuDispatcher = nullptr;
//        PxCudaContextManager* cudaContextManager = nullptr;
        px::PxScene *m_scene = nullptr;
        px::PxControllerManager *m_controllerManager = nullptr;
        px::PxCooking *m_cooking = nullptr;
        std::unordered_map<std::string, px::PxMaterial *> m_materials;
    };
}

#endif //ACTIO_PHYSIC_MANAGER_H
