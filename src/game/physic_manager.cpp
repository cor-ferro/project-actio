#include "physic_manager.h"

game::PhysicManager::PhysicManager() {
    init();
}

game::PhysicManager::~PhysicManager() {
    destroy();
}

void game::PhysicManager::init() {
    m_foundation = PxCreateFoundation(PX_FOUNDATION_VERSION, m_allocator, m_errorCallback);
    m_pvd = PxCreatePvd(*m_foundation);
    m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, px::PxTolerancesScale(), true, m_pvd);

    m_cpuDispatcher = px::PxDefaultCpuDispatcherCreate(4);

//    PxCudaContextManagerDesc cudaContextManagerDesc;
//    cudaContextManager = PxCreateCudaContextManager(*m_foundation, cudaContextManagerDesc);

    px::PxSceneDesc sceneDesc(m_physics->getTolerancesScale());
    sceneDesc.gravity = px::PxVec3(0.0f, -0.1f, 0.0f);
    sceneDesc.cpuDispatcher = m_cpuDispatcher;
//    sceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;
//    sceneDesc.flags |= px::PxSceneFlag::eENABLE_GPU_DYNAMICS;
//    sceneDesc.m_gpuDispatcher = cudaContextManager->getGpuDispatcher();
//    sceneDesc.filterShader = px::PxDefaultSimulationFilterShader;
//    sceneDesc.filterShader = PhysicFilterShader;
    sceneDesc.filterShader = px::PxDefaultSimulationFilterShader;
    sceneDesc.flags |= px::PxSceneFlag::eENABLE_PCM;
    sceneDesc.flags |= px::PxSceneFlag::eENABLE_STABILIZATION;
    sceneDesc.flags |= px::PxSceneFlag::eENABLE_ACTIVE_ACTORS;
    sceneDesc.gpuMaxNumPartitions = 8;
    sceneDesc.simulationEventCallback = this;

    m_scene = m_physics->createScene(sceneDesc);

    px::PxPvdSceneClient* pvdClient = m_scene->getScenePvdClient();
    if(pvdClient)
    {
        pvdClient->setScenePvdFlag(px::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(px::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(px::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }

    m_scene->setVisualizationParameter(px::PxVisualizationParameter::eSCALE, 1.0f);
    m_scene->setVisualizationParameter(px::PxVisualizationParameter::eACTOR_AXES, 1.0f);
    m_scene->setVisualizationParameter(px::PxVisualizationParameter::eWORLD_AXES, 1.0f);
    m_scene->setVisualizationParameter(px::PxVisualizationParameter::eBODY_AXES, 1.0f);
    m_scene->setVisualizationParameter(px::PxVisualizationParameter::eBODY_MASS_AXES, 1.0f);
    m_scene->setVisualizationParameter(px::PxVisualizationParameter::eBODY_LIN_VELOCITY, 1.0f);
    m_scene->setVisualizationParameter(px::PxVisualizationParameter::eBODY_ANG_VELOCITY, 1.0f);
//    m_scene->setVisualizationParameter(px::PxVisualizationParameter::eCONTACT_POINT, 1.0f);
//    m_scene->setVisualizationParameter(px::PxVisualizationParameter::eCONTACT_NORMAL, 1.0f);
//    m_scene->setVisualizationParameter(px::PxVisualizationParameter::eCONTACT_ERROR, 1.0f);
//    m_scene->setVisualizationParameter(px::PxVisualizationParameter::eCONTACT_FORCE, 1.0f);
//    m_scene->setVisualizationParameter(px::PxVisualizationParameter::eCOLLISION_AABBS, 1.0f);
    m_scene->setVisualizationParameter(px::PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
    m_scene->setVisualizationParameter(px::PxVisualizationParameter::eCOLLISION_AXES, 1.0f);
    m_scene->setVisualizationParameter(px::PxVisualizationParameter::eCOLLISION_EDGES, 1.0f);
//    m_scene->setVisualizationParameter(px::PxVisualizationParameter::eCOLLISION_STATIC, 1.0f);
//    m_scene->setVisualizationParameter(px::PxVisualizationParameter::eCOLLISION_DYNAMIC, 1.0f);
    m_scene->setVisualizationParameter(px::PxVisualizationParameter::eJOINT_LIMITS, 1.0f);
//    m_scene->setVisualizationParameter(px::PxVisualizationParameter::eCULL_BOX, 1.0f);

    m_controllerManager = PxCreateControllerManager(*m_scene);

    px::PxTolerancesScale scale;
    px::PxCookingParams params(scale);
    params.meshWeldTolerance = 0.001f;
    params.meshPreprocessParams = px::PxMeshPreprocessingFlags(px::PxMeshPreprocessingFlag::eWELD_VERTICES);
    params.buildGPUData = false; //Enable GRB data being produced in cooking.

    m_cooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_foundation, params);
    if (!m_cooking)
        console::err("PxCreateCooking failed!");

    m_materials.insert({"default", m_physics->createMaterial(0.2f, 0.1f, 0.2f)});

    px::PxRigidStatic *groundPlane = PxCreatePlane(*m_physics, px::PxPlane({0, 1, 0}, 0), *(m_materials["default"]));

    m_scene->addActor(*groundPlane);

    console::info("system physics inited");
}

void game::PhysicManager::destroy() {
    console::info("destroy physic system");
    m_materials.erase(m_materials.begin(), m_materials.end());

    if (m_controllerManager != nullptr) {
        console::info("destroy controller manager");
        m_controllerManager->purgeControllers();
        m_controllerManager->release();
        m_controllerManager = nullptr;
        console::info("end destroy controller manager");
    }

    if (m_cpuDispatcher != nullptr) {
        m_cpuDispatcher->release();
        m_cpuDispatcher = nullptr;
    }

    if (m_gpuDispatcher != nullptr) {
//        m_gpuDispatcher->release();
    }

//    if (cudaContextManager != nullptr) {
//        cudaContextManager->release();
//    }

    if (m_scene != nullptr) {
        m_scene->release();
        m_scene = nullptr;
    }

    if (m_cooking != nullptr) {
        m_cooking->release();
        m_cooking = nullptr;
    }

    if (m_physics != nullptr) {
        m_physics->release();
        m_physics = nullptr;
    }

    if (m_foundation != nullptr) {
        m_foundation->release();
        m_foundation = nullptr;
    }
}

void game::PhysicManager::update(const px::PxReal& dt) {
//    m_scene->collide(dt);
//    m_scene->fetchCollision(true);
//    m_scene->advance();
//    m_scene->fetchResults(true);

    m_scene->simulate(1.0f/60.0f);
    m_scene->fetchResults(true);
}

physx::PxPhysics *const game::PhysicManager::getPhysics() {
    return m_physics;
}

physx::PxScene *const game::PhysicManager::getScene() {
    return m_scene;
}

physx::PxControllerManager *const game::PhysicManager::getControllerManager() {
    return m_controllerManager;
}

physx::PxCooking *const game::PhysicManager::getCooking() {
    return m_cooking;
}

void game::PhysicManager::setSceneGravity(const vec3& value) {
    m_scene->setGravity(px::PxVec3(value.x, value.y, value.z));
}

const physx::PxVec3 game::PhysicManager::getSceneGravity() {
    return m_scene->getGravity();
}

void game::PhysicManager::onContact(const px::PxContactPairHeader& pairHeader, const px::PxContactPair *pairs,
                                    px::PxU32 nbPairs) {
    console::info("onContact");
}

void game::PhysicManager::onTrigger(px::PxTriggerPair *pairs, px::PxU32 count) {
    console::info("onTrigger");
}

void game::PhysicManager::onConstraintBreak(px::PxConstraintInfo *, px::PxU32) {
    console::info("onConstraintBreak");
}

void game::PhysicManager::onWake(px::PxActor **, px::PxU32) {
    console::info("onWake");
}

void game::PhysicManager::onSleep(px::PxActor **, px::PxU32) {
    console::info("onSleep");
}

void game::PhysicManager::onAdvance(const px::PxRigidBody *const *, const px::PxTransform *, const px::PxU32) {
    console::info("onAdvance");
}

game::px::PxMaterial *const game::PhysicManager::getMaterial() {
    return m_materials.find("default")->second;
}

game::px::PxMaterial *const game::PhysicManager::getMaterial(const std::string& name) {
    auto it = m_materials.find(name);

    if (it == m_materials.end()) {
        return m_materials.find("default")->second;
    }

    return it->second;
}

void game::PhysicManager::wave(const vec3& position, const vec3& direction) {
    float radius = 1.0f;

    px::PxSphereGeometry geometry(5.0f);
    px::PxTransform pose(px::PxVec3(position.x, position.y, position.z));
//                px::PxOverlapBuffer buf;

    px::PxOverlapHit hitBuffer[5];
    px::PxMemZero(&hitBuffer, sizeof(hitBuffer));
    px::PxFilterData fd(0, 0, 0, 0);
    px::PxOverlapBuffer buf(hitBuffer, 5);

    bool status = m_scene->overlap(geometry, pose, buf, px::PxQueryFilterData(fd, px::PxQueryFlag::eDYNAMIC));

    console::info("hits: %i", buf.getNbAnyHits());
    console::info("touch: %i", buf.getNbTouches());
    console::info("buf.hasBlock: %i", buf.hasBlock);

    if (status) {
        const px::PxU32 countHits = buf.getNbAnyHits();

        for (px::PxU32 i = 0; i < countHits; i++) {
            const px::PxOverlapHit& hit = buf.getAnyHit(i);

//                        px::PxU32 flags = hit.actor->getActorFlags();
//                        if (flags & px::PxRigidBodyFlag::eKINEMATIC) {
//                            console::info("skip");
////                            continue;
//                        }
//
//                        auto* body = static_cast<px::PxRigidDynamic*>(hit.actor);
//
//                        px::PxVec3 dir = pose.p - hit.shape->;
//
//                        body->addForce(px::PxVec3(0.0f, 15.0f, 0.0f), px::PxForceMode::eVELOCITY_CHANGE);
//                        console::info("add force!");
        }

    }


//                px::PxSweepBuffer hit;
//                px::PxGeometry sweepShape = px::PxCreate //px::PxSphereGeometry(static_cast<px::PxReal>(radius));
//                px::PxTransform initialPose(px::PxVec3(position.x, position.y, position.z));
//                px::PxVec3 sweepDirection(direction.x, direction.y, direction.z);
//
//                px::PxMaterial *gMaterial = m_materials.find("default")->second;
//                px::PxRigidStatic *actor = px::PxCreateStatic(*m_physics, initialPose, sweepShape, *gMaterial);



//                const px::PxSphereGeometry &sphereGeom = static_cast<const px::PxSphereGeometry &>(sweepShape);
//
//                m_scene->removeActor(*actor);

//                PxRigidDynamic* actor = mActor->getController()->getActor();
//                PxShape* capsuleShape = getShape( *actor );
//                px::PxCapsuleGeometry capGeom(capsuleShape->getGeometry().capsule());
//
//                bool status = m_scene->sweep(sweepShape, initialPose, sweepDirection, distance, hit);

//                if (status) {
//                    px::PxU32 countHits = hit.getNbTouches();
//
//                    for (px::PxU32 i = 0; i < countHits; i++) {
//                        const px::PxSweepHit &h = hit.getTouch(i);
//                        const px::PxType actorType = h.actor->getConcreteType();
//
//                        if (actorType == px::PxConcreteType::eRIGID_DYNAMIC) {
//                            auto* body = reinterpret_cast<px::PxRigidDynamic*>(h.actor);
//                            body->addForce(px::PxVec3(0.0f, 1.0f, 0.0f), px::PxForceMode::eVELOCITY_CHANGE);
//                        }
//                    }
//                }
}

void game::PhysicManager::applyForce(physx::PxRigidDynamic *const actor, const vec3& direction) {
    actor->addForce(px::PxVec3(direction.x, direction.y, direction.z), px::PxForceMode::eVELOCITY_CHANGE);
}

void game::PhysicManager::applyForce(ex::Entity entity, const vec3& direction) {
    ex::ComponentHandle<components::PhysicActor> physic = entity.component<components::PhysicActor>();

    applyForce(physic->getDynamicActor(), direction);
}

physx::PxRigidDynamic *game::PhysicManager::createDynamicActor(const px::PxTransform& transform) {
    return getPhysics()->createRigidDynamic(transform);
}

physx::PxRigidDynamic *game::PhysicManager::createDynamicActor(const px::PxVec3& pos) {
    return getPhysics()->createRigidDynamic(px::PxTransform(pos));
}

physx::PxRigidDynamic *game::PhysicManager::createDynamicActor(const vec3& pos) {
    return getPhysics()->createRigidDynamic(px::PxTransform(px::PxVec3(pos.x, pos.y, pos.z)));
}

physx::PxRigidStatic *game::PhysicManager::createStaticActor(const physx::PxTransform& transform) {
    return getPhysics()->createRigidStatic(transform);
}

physx::PxRigidStatic *game::PhysicManager::createStaticActor(const physx::PxVec3& pos) {
    return getPhysics()->createRigidStatic(px::PxTransform(pos));
}

physx::PxRigidStatic *game::PhysicManager::createStaticActor(const vec3& pos) {
    return getPhysics()->createRigidStatic(px::PxTransform(px::PxVec3(pos.x, pos.y, pos.z)));
}

void game::PhysicManager::addToScene(physx::PxRigidActor *actor) {
    m_scene->addActor(*actor);
}

void game::PhysicManager::removeFromScene(physx::PxRigidActor *actor) {
    m_scene->removeActor(*actor);
}

void game::PhysicManager::createPlaneGeometry(physx::PxRigidActor *actor) {
    px::PxRigidActorExt::createExclusiveShape(*actor, px::PxPlaneGeometry(), *getMaterial());
}

void game::PhysicManager::createSphereGeometry(physx::PxRigidActor *actor, const px::PxReal& radius) {
    px::PxRigidActorExt::createExclusiveShape(*actor, px::PxSphereGeometry(radius), *getMaterial());
}

void game::PhysicManager::createBoxGeometry(physx::PxRigidActor *actor, const physx::PxVec3& size) {
    px::PxRigidActorExt::createExclusiveShape(*actor, px::PxBoxGeometry(size.x, size.y, size.z), *getMaterial());
}

void
game::PhysicManager::createCapsuleGeometry(physx::PxRigidActor *actor, const px::PxReal& radius,
                                           const px::PxReal& halfHeight) {
    px::PxRigidActorExt::createExclusiveShape(*actor, px::PxCapsuleGeometry(radius, halfHeight), *getMaterial());
}
