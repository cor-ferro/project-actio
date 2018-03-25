//
// Created by demitriy on 3/24/18.
//

#ifndef ACTIO_PHYSIC_H
#define ACTIO_PHYSIC_H

#include "entityx/entityx.h"
#include "PxPhysics.h"
#include "PxScene.h"
#include "PxRigidDynamic.h"
#include "PxShape.h"
#include "PxPhysicsAPI.h"
#include "../components/physic.h"
#include "../components/transform.h"
#include "../../lib/console.h"
#include "../components/controlled.h"

namespace game {
    namespace systems {
        using namespace entityx;
        using namespace physx;

        class Physic : public entityx::System<Physic>, public Receiver<Physic> {
        public:
            Physic() {
                gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gAllocator, gErrorCallback);
                gPvd = PxCreatePvd(*gFoundation);
                gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

                cpuDispatcher = PxDefaultCpuDispatcherCreate(4);

//                PxCudaContextManagerDesc cudaContextManagerDesc;
//                cudaContextManager = PxCreateCudaContextManager(*gFoundation, cudaContextManagerDesc);

                PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
                sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);
                sceneDesc.cpuDispatcher = cpuDispatcher;
//                sceneDesc.gpuDispatcher = cudaContextManager->getGpuDispatcher();
                sceneDesc.filterShader = PxDefaultSimulationFilterShader;
//                sceneDesc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;
                sceneDesc.flags |= PxSceneFlag::eENABLE_PCM;
                sceneDesc.flags |= PxSceneFlag::eENABLE_STABILIZATION;
//                sceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;
                sceneDesc.gpuMaxNumPartitions = 8;

                gScene = gPhysics->createScene(sceneDesc);
                gControllerManager = PxCreateControllerManager(*gScene);

                pxMaterials.insert({ "default", gPhysics->createMaterial(0.5f, 0.5f, 0.6f) });

                console::info("system physics inited");
            }

            ~Physic() {
                console::info("destroy physic system");
                pxMaterials.erase(pxMaterials.begin(), pxMaterials.end());

                if (gControllerManager != nullptr) {
                    console::info("destroy controller manager");
                    gControllerManager->purgeControllers();
                    gControllerManager->release();
                    gControllerManager = nullptr;
                    console::info("end destroy controller manager");
                }

                if (cpuDispatcher != nullptr) {
                    cpuDispatcher->release();
                    cpuDispatcher = nullptr;
                }

                if (gpuDispatcher != nullptr) {
//                    gpuDispatcher->release();
                }

//                if (cudaContextManager != nullptr) {
//                    cudaContextManager->release();
//                }

                if (gScene != nullptr) {
                    gScene->release();
                    gScene = nullptr;
                }

                if (gPhysics != nullptr) {
                    gPhysics->release();
                    gPhysics = nullptr;
                }

                if (gFoundation != nullptr) {
                    gFoundation->release();
                    gFoundation = nullptr;
                }

//
//                console::info("physics released");
            }

            void destroy() {

            }

            void update(EntityManager &es, EventManager &events, TimeDelta dt) override {
                PX_UNUSED(false);

                gScene->simulate(static_cast<PxReal>(dt / 1000.0));
                gScene->fetchResults(true);

                es.each<components::Physic, components::Transform>([](Entity entity, components::Physic& physic, components::Transform& transform) {
                     if (!physic.dynamic->isSleeping()) {
                         PxTransform newTransform = physic.dynamic->getGlobalPose();

                         transform.setPosition(newTransform.p.x, newTransform.p.y, newTransform.p.z);
                         transform.setQuaternion(newTransform.q.x, newTransform.q.y, newTransform.q.z, newTransform.q.w);
                     }
                });
            }

            void add() {
                PxMaterial* gMaterial = pxMaterials.find("default")->second;

                PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *gMaterial);
                gScene->addActor(*groundPlane);

                PxRigidDynamic* ball = createDynamic(PxTransform(PxVec3(0, 20, 0)), PxSphereGeometry(3), PxVec3(0, -25, -5));
                PxRigidBodyExt::updateMassAndInertia(*ball, 1000.0f);
                gScene->addActor(*ball);
            }

            PxRigidDynamic* createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity) {
                PxMaterial* gMaterial = pxMaterials.find("default")->second;

                PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, t, geometry, *gMaterial, 10.0f);
                dynamic->setAngularDamping(0.5f);
                dynamic->setLinearVelocity(velocity);

                return dynamic;
            }

            PxControllerManager* const getControllerManager() {
                return gControllerManager;
            }

            void configure(entityx::EventManager &event_manager) {
                event_manager.subscribe<EntityCreatedEvent>(*this);
                event_manager.subscribe<EntityDestroyedEvent>(*this);
            }

            void receive(const EntityCreatedEvent &event) {
//                const Entity& e = event.entity;
//                event.entity.assign<components::Controlled>(gControllerManager);
            }

            void receive(const EntityDestroyedEvent &event) {
//                event.entity.assign<components::Controlled>(gControllerManager);
            }

        private:
            PxDefaultAllocator gAllocator;
            PxDefaultErrorCallback gErrorCallback;
            PxFoundation* gFoundation = nullptr;
            PxPhysics*  gPhysics = nullptr;
            PxPvd* gPvd = nullptr;
            PxDefaultCpuDispatcher* cpuDispatcher = nullptr;
            PxGpuDispatcher* gpuDispatcher = nullptr;
//            PxCudaContextManager* cudaContextManager = nullptr;
            PxScene* gScene = nullptr;
            PxControllerManager* gControllerManager = nullptr;
            std::unordered_map<std::string, PxMaterial*> pxMaterials;
        };
    }
}

#endif //ACTIO_PHYSIC_H
