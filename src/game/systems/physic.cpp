#include "physic.h"
#include "../world.h"
#include <glm/gtc/random.hpp>

namespace game {
    namespace systems {
        Physic::Physic(World *world) : systems::BaseSystem(world) {
            gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gAllocator, gErrorCallback);
            gPvd = PxCreatePvd(*gFoundation);
            gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, px::PxTolerancesScale(), true, gPvd);

            cpuDispatcher = px::PxDefaultCpuDispatcherCreate(4);

//                PxCudaContextManagerDesc cudaContextManagerDesc;
//                cudaContextManager = PxCreateCudaContextManager(*gFoundation, cudaContextManagerDesc);

            px::PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
            sceneDesc.gravity = px::PxVec3(0.0f, -9.8f, 0.0f);
            sceneDesc.cpuDispatcher = cpuDispatcher;
//                sceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;
//                sceneDesc.flags |= px::PxSceneFlag::eENABLE_GPU_DYNAMICS;
//                sceneDesc.gpuDispatcher = cudaContextManager->getGpuDispatcher();
//                sceneDesc.filterShader = px::PxDefaultSimulationFilterShader;
            sceneDesc.filterShader = PhysicFilterShader;
            sceneDesc.flags |= px::PxSceneFlag::eENABLE_PCM;
            sceneDesc.flags |= px::PxSceneFlag::eENABLE_STABILIZATION;
            sceneDesc.gpuMaxNumPartitions = 8;
            sceneDesc.simulationEventCallback = this;

            gScene = gPhysics->createScene(sceneDesc);
            gScene->setVisualizationParameter(px::PxVisualizationParameter::eSCALE, 1.0f);
            gScene->setVisualizationParameter(px::PxVisualizationParameter::eACTOR_AXES, 1.0f);
            gScene->setVisualizationParameter(px::PxVisualizationParameter::eWORLD_AXES, 1.0f);
            gScene->setVisualizationParameter(px::PxVisualizationParameter::eBODY_AXES, 1.0f);
            gScene->setVisualizationParameter(px::PxVisualizationParameter::eBODY_MASS_AXES, 1.0f);
            gScene->setVisualizationParameter(px::PxVisualizationParameter::eBODY_LIN_VELOCITY, 1.0f);
            gScene->setVisualizationParameter(px::PxVisualizationParameter::eBODY_ANG_VELOCITY, 1.0f);
            gScene->setVisualizationParameter(px::PxVisualizationParameter::eCONTACT_POINT, 1.0f);
            gScene->setVisualizationParameter(px::PxVisualizationParameter::eCONTACT_NORMAL, 1.0f);
            gScene->setVisualizationParameter(px::PxVisualizationParameter::eCONTACT_ERROR, 1.0f);
            gScene->setVisualizationParameter(px::PxVisualizationParameter::eCONTACT_FORCE, 1.0f);
            gScene->setVisualizationParameter(px::PxVisualizationParameter::eCOLLISION_AABBS, 1.0f);
//                gScene->setVisualizationParameter(px::PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
            gScene->setVisualizationParameter(px::PxVisualizationParameter::eCOLLISION_AXES, 1.0f);
            gScene->setVisualizationParameter(px::PxVisualizationParameter::eCOLLISION_EDGES, 1.0f);
//                gScene->setVisualizationParameter(px::PxVisualizationParameter::eCOLLISION_STATIC, 1.0f);
//                gScene->setVisualizationParameter(px::PxVisualizationParameter::eCOLLISION_DYNAMIC, 1.0f);
            gScene->setVisualizationParameter(px::PxVisualizationParameter::eJOINT_LIMITS, 1.0f);
            gScene->setVisualizationParameter(px::PxVisualizationParameter::eCULL_BOX, 1.0f);

            gControllerManager = PxCreateControllerManager(*gScene);

            px::PxTolerancesScale scale;
            px::PxCookingParams params(scale);
            params.meshWeldTolerance = 0.001f;
            params.meshPreprocessParams = px::PxMeshPreprocessingFlags(px::PxMeshPreprocessingFlag::eWELD_VERTICES);
            params.buildGPUData = false; //Enable GRB data being produced in cooking.
            cooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, params);
            if(!cooking)
                console::err("PxCreateCooking failed!");

            pxMaterials.insert({"default", gPhysics->createMaterial(0.2f, 0.1f, 0.2f)});

            px::PxRigidStatic *groundPlane = PxCreatePlane(*gPhysics, px::PxPlane(0, 1, 0, 0),
                                                           *(pxMaterials["default"]));
            gScene->addActor(*groundPlane);

            controlDir = px::PxVec3(0.0f);

            console::info("system physics inited");
        }

        Physic::~Physic() {
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

            if (cooking != nullptr) {
                cooking->release();
                cooking = nullptr;
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

        void Physic::configure(ex::EntityManager &entities, entityx::EventManager &event_manager) {
            event_manager.subscribe<ex::EntityCreatedEvent>(*this);
            event_manager.subscribe<ex::EntityDestroyedEvent>(*this);
            event_manager.subscribe<events::PhysicCreateSphere>(*this);
            event_manager.subscribe<events::PhysicCreateBox>(*this);
            event_manager.subscribe<events::PhysicForce>(*this);
            event_manager.subscribe<events::SetupControlled>(*this);
            event_manager.subscribe<events::KeyPress>(*this);

            {
                debugLinesEntity = entities.create();

                Mesh *mesh = Mesh::Create();

                GeometryPrimitive::Lines(mesh->geometry, {});
                mesh->geometry.setType(Geometry::Geometry_Dynamic);
                mesh->material->setDiffuse(vec3(0.0f, 1.0f, 0.0f));

                mesh->setDrawType(Mesh_Draw_Line);

                debugLinesEntity.assign<components::Model>(mesh);
                debugLinesEntity.assign<components::Transform>(vec3(0.0f));
            }

            {
                debugTrianglesEntity = entities.create();

                Mesh *mesh = Mesh::Create();

                mesh->geometry.setType(Geometry::Geometry_Dynamic);
                mesh->material->setDiffuse(vec3(0.0f, 0.0f, 1.0f));

                mesh->setDrawType(Mesh_Draw_Triangle);

                debugTrianglesEntity.assign<components::Model>(mesh);
                debugTrianglesEntity.assign<components::Transform>(vec3(0.0f));
            }

            {
                debugPointsEntity = entities.create();

                Mesh *mesh = Mesh::Create();

                mesh->geometry.setType(Geometry::Geometry_Dynamic);
                mesh->material->setDiffuse(vec3(1.0f, 0.0f, 0.0f));

                mesh->setDrawType(Mesh_Draw_Point);

                debugPointsEntity.assign<components::Model>(mesh);
                debugPointsEntity.assign<components::Transform>(vec3(0.0f));
            }
        }

        void Physic::postConfigure(entityx::EventManager &events) {
            auto modelLines = components::get<components::Model>(debugLinesEntity);
            auto modelTriangles = components::get<components::Model>(debugTrianglesEntity);
            auto modelPoints = components::get<components::Model>(debugPointsEntity);

            events.emit<events::RenderSetupModel>(debugLinesEntity);
            events.emit<events::RenderSetupModel>(debugTrianglesEntity);
            events.emit<events::RenderSetupModel>(debugPointsEntity);
        }

        void Physic::destroy() {

        }

        void Physic::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
            PX_UNUSED(false);

            auto elapsedTime = static_cast<px::PxReal>(dt / 1000.0);

            gScene->collide(elapsedTime);
            gScene->fetchCollision(true);
            gScene->advance();
            gScene->fetchResults(true);

            while (!contactedActors.empty()) {
                const std::pair<px::PxActor *, px::PxActor *> &pair = contactedActors.top();

                events.emit<events::PhysicContact>(pair.first, pair.second);

                contactedActors.pop();
            }

            es.each<components::Physic, components::Transform>(
                [](
                    ex::Entity entity,
                    components::Physic &physic,
                    components::Transform &transform
                ) {
                    if (physic.actor == nullptr) {
                        return;
                    }

                    switch (physic.type) {
                        case components::Physic::DynamicObject: {
                            auto *actor = static_cast<px::PxRigidDynamic *>(physic.actor);
                            if (actor->isSleeping()) {
                                return;
                            }
                        }
                        case components::Physic::StaticObject: {
                            px::PxTransform newTransform = physic.actor->getGlobalPose();
                            transform.setTransform(newTransform);
                        }
                        default: return;
                    }
                }
            );


            es.each<c::Character, c::UserControl, c::Transform>([&elapsedTime, this](
                    ex::Entity entity,
                    c::Character &character,
                    c::UserControl &userControl,
                    c::Transform &transform
            ) {
                px::PxVec3 gravity = gScene->getGravity();

                if (character.isJump) {
                    character.jump += (gravity.y) * elapsedTime;
                    character.motion.y += 0.1f * character.jump;
                }

                const px::PxControllerCollisionFlags flags = userControl.controller->move(
                        PX_REST_VEC(character.motion), 0.01f, elapsedTime,
                        px::PxControllerFilters()
                );

                if (flags & px::PxControllerCollisionFlag::eCOLLISION_DOWN) {
                    character.isJump = false;
                    character.jump = 0.0f;
                    character.motion.y = 0.0f;
                }

//                    if (flags & px::PxControllerCollisionFlag::eCOLLISION_UP)
//                        console::info("eCOLLISION_UP");
//                    if (flags & px::PxControllerCollisionFlag::eCOLLISION_SIDES)
//                        console::info("eCOLLISION_SIDES");

                const px::PxExtendedVec3 &pxPos = userControl.controller->getFootPosition();

                if (!character.isJump && pxPos.y > 0.0) {
                    character.jump -= (gravity.y) * elapsedTime;
                    character.motion.y -= 0.1f * character.jump;
                }

                transform.setPosition(px::toVec3(pxPos));
            });

            if (drawDebug) {
                drawDebugBuffer(events);
            }
        }

        void Physic::add() {
            px::PxRigidDynamic *ball = createDynamic(
                    px::PxTransform(px::PxVec3(0, 20, 0)),
                    px::PxSphereGeometry(3),
                    px::PxVec3(0, -25, -5)
            );
            px::PxRigidBodyExt::updateMassAndInertia(*ball, 1000.0f);
            gScene->addActor(*ball);
        }

        void Physic::setDrawDebug(bool value) {
            drawDebug = value;

            if (!drawDebug) {
                auto renderableLines = components::get<components::Renderable>(debugLinesEntity);
                auto renderableTriangles = components::get<components::Renderable>(debugTrianglesEntity);
                auto renderablePoints = components::get<components::Renderable>(debugPointsEntity);

                renderableLines.remove();
                renderableTriangles.remove();
                renderablePoints.remove();
            } else {
                debugLinesEntity.assign<components::Renderable>();
                debugTrianglesEntity.assign<components::Renderable>();
                debugPointsEntity.assign<components::Renderable>();
            }
        }

        void Physic::drawDebugBuffer(ex::EventManager &events) {
            const px::PxRenderBuffer &rb = gScene->getRenderBuffer();

            auto linesModel = components::get<components::Model>(debugLinesEntity);
            Mesh *linesMesh = linesModel->getMesh();

            auto trianglesModel = components::get<components::Model>(debugTrianglesEntity);
            Mesh *trianglesMesh = trianglesModel->getMesh();

            auto pointsModel = components::get<components::Model>(debugPointsEntity);
            Mesh *pointsMesh = pointsModel->getMesh();

            const px::PxU32 countLines = rb.getNbLines();
            const px::PxU32 countTriangles = rb.getNbTriangles();
            const px::PxU32 countPoints = rb.getNbPoints();

            {
                if (countLines * 2 != linesMesh->geometry.getCountVertices()) {
                    std::vector<vec3> vertices;
                    vertices.reserve(countLines * 2);

                    for (px::PxU32 i = 0; i < countLines; i++) {
                        const px::PxDebugLine &line = rb.getLines()[i];

                        vertices.push_back(vec3(line.pos0.x, line.pos0.y, line.pos0.z));
                        vertices.push_back(vec3(line.pos1.x, line.pos1.y, line.pos1.z));
                    }

                    linesMesh->geometry.setVertices(vertices);
                    linesMesh->geometry.setNeedUpdateVertices(true);
                } else {
                    GeometryVertices *vertices = linesMesh->geometry.getVertices();

                    for (px::PxU32 i = 0; i < countLines; i++) {
                        const px::PxDebugLine &line = rb.getLines()[i];

                        Vertex &vertex = vertices->at(i * 2);
                        vertex.Position = vec3(line.pos0.x, line.pos0.y, line.pos0.z);

                        Vertex &vertex2 = vertices->at((i * 2) + 1);
                        vertex2.Position = vec3(line.pos1.x, line.pos1.y, line.pos1.z);
                    }
                }
            }

            {
                if (countTriangles * 3 != trianglesMesh->geometry.getCountVertices()) {
                    std::vector<vec3> vertices;
                    vertices.reserve(countTriangles * 3);

                    for (px::PxU32 i = 0; i < countTriangles; i++) {
                        const px::PxDebugTriangle &triangle = rb.getTriangles()[i];

                        vertices.push_back(vec3(triangle.pos0.x, triangle.pos0.y, triangle.pos0.z));
                        vertices.push_back(vec3(triangle.pos1.x, triangle.pos1.y, triangle.pos1.z));
                        vertices.push_back(vec3(triangle.pos2.x, triangle.pos2.y, triangle.pos2.z));
                    }

                    trianglesMesh->geometry.setVertices(vertices);
                    trianglesMesh->geometry.setNeedUpdateVertices(true);
                } else {
                    GeometryVertices *vertices = trianglesMesh->geometry.getVertices();

                    for (px::PxU32 i = 0; i < countTriangles; i++) {
                        const px::PxDebugTriangle &triangle = rb.getTriangles()[i];

                        Vertex &vertex = vertices->at(i * 3);
                        vertex.Position = vec3(triangle.pos0.x, triangle.pos0.y, triangle.pos0.z);

                        Vertex &vertex2 = vertices->at((i * 3) + 1);
                        vertex2.Position = vec3(triangle.pos1.x, triangle.pos1.y, triangle.pos1.z);

                        Vertex &vertex3 = vertices->at((i * 3) + 2);
                        vertex3.Position = vec3(triangle.pos2.x, triangle.pos2.y, triangle.pos2.z);
                    }
                }
            }

            {
                if (countPoints != pointsMesh->geometry.getCountVertices()) {
                    std::vector<vec3> vertices;
                    vertices.reserve(countPoints);

                    for (px::PxU32 i = 0; i < countPoints; i++) {
                        const px::PxDebugPoint &point = rb.getPoints()[i];

                        vertices.push_back(vec3(point.pos.x, point.pos.y, point.pos.z));
                    }

                    pointsMesh->geometry.setVertices(vertices);
                    pointsMesh->geometry.setNeedUpdateVertices(true);
                } else {
                    GeometryVertices *vertices = pointsMesh->geometry.getVertices();

                    for (px::PxU32 i = 0; i < countPoints; i++) {
                        const px::PxDebugPoint &point = rb.getPoints()[i];

                        Vertex &vertex = vertices->at(i);
                        vertex.Position = vec3(point.pos.x, point.pos.y, point.pos.z);
                    }
                }
            }

            events.emit<events::RenderSetupModel>(debugLinesEntity, events::RenderSetupModel::Action::Update);
            events.emit<events::RenderSetupModel>(debugTrianglesEntity, events::RenderSetupModel::Action::Update);
            events.emit<events::RenderSetupModel>(debugPointsEntity, events::RenderSetupModel::Action::Update);
        }

        void Physic::setSceneGravity(vec3 value) {
            gScene->setGravity(PX_REST_VEC(value));
        };

        px::PxRigidDynamic * Physic::createDynamic(const px::PxTransform &t, const px::PxGeometry &geometry, const px::PxVec3 &velocity) {
            px::PxMaterial *gMaterial = pxMaterials.find("default")->second;

            px::PxRigidDynamic *dynamic = PxCreateDynamic(*gPhysics, t, geometry, *gMaterial, 10.0f);
            dynamic->setAngularDamping(0.5f);
            dynamic->setLinearVelocity(velocity);

            return dynamic;
        }

        void Physic::wave(vec3 position, vec3 direction) {
            float radius = 1.0f;

            px::PxSphereGeometry geometry(5.0f);
            px::PxTransform pose(PX_REST_VEC(position));
//                px::PxOverlapBuffer buf;

            px::PxOverlapHit hitBuffer[5];
            px::PxMemZero(&hitBuffer, sizeof(hitBuffer));
            px::PxFilterData fd(0, 0, 0, 0);
            px::PxOverlapBuffer buf(hitBuffer, 5);

            bool status = gScene->overlap(geometry, pose, buf, px::PxQueryFilterData(fd, px::PxQueryFlag::eDYNAMIC));

            console::info("hits: %i", buf.getNbAnyHits());
            console::info("touch: %i", buf.getNbTouches());
            console::info("buf.hasBlock: %i", buf.hasBlock);

            if (status) {
                const px::PxU32 countHits = buf.getNbAnyHits();

                for (px::PxU32 i = 0; i < countHits; i++) {
                    const px::PxOverlapHit &hit = buf.getAnyHit(i);

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
//                px::PxTransform initialPose(PX_REST_VEC(position));
//                px::PxVec3 sweepDirection = PX_REST_VEC(direction);
//
//                px::PxMaterial *gMaterial = pxMaterials.find("default")->second;
//                px::PxRigidStatic *actor = px::PxCreateStatic(*gPhysics, initialPose, sweepShape, *gMaterial);



//                const px::PxSphereGeometry &sphereGeom = static_cast<const px::PxSphereGeometry &>(sweepShape);
//
//                gScene->removeActor(*actor);

//                PxRigidDynamic* actor = mActor->getController()->getActor();
//                PxShape* capsuleShape = getShape( *actor );
//                px::PxCapsuleGeometry capGeom(capsuleShape->getGeometry().capsule());
//
//                bool status = gScene->sweep(sweepShape, initialPose, sweepDirection, distance, hit);

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

        void Physic::receive(const ex::EntityCreatedEvent &event) {
//                const Entity& e = event.entity;
//                event.entity.assign<components::Controlled>(gControllerManager);
        }

        void Physic::receive(const ex::EntityDestroyedEvent &event) {
//                event.entity.assign<components::Controlled>(gControllerManager);
        }

        void Physic::receive(const events::PhysicCreateSphere &event) {
            ex::Entity entity = event.entity;

            ex::ComponentHandle<components::Transform> transform = entity.component<components::Transform>();

            px::PxMaterial *gMaterial = pxMaterials.find("default")->second;
            const px::PxTransform pxTransform(PX_REST_VEC(transform->position));
            const px::PxSphereGeometry pxGeometry(event.radius);

            px::PxRigidDynamic *dynamic = PxCreateDynamic(*gPhysics, pxTransform, pxGeometry, *gMaterial, 100.0f);
            dynamic->setAngularDamping(10.5f);
            dynamic->setLinearVelocity(px::PxVec3(0, 0, 0));

            if (entity.has_component<c::PhysicEntity>()) {
                auto physicEntityHandle = components::get<c::PhysicEntity>(entity);

                dynamic->userData = static_cast<void *>(physicEntityHandle.get());
            }

            px::PxRigidBodyExt::updateMassAndInertia(*dynamic, 100.0f);

            gScene->addActor(*dynamic);

            entity.assign<components::Physic>(dynamic);
        }

        void Physic::receive(const events::PhysicCreateBox &event) {
            ex::Entity entity = event.entity;

            ex::ComponentHandle<components::Transform> transform = entity.component<components::Transform>();

            px::PxMaterial *gMaterial = pxMaterials.find("default")->second;
            const px::PxTransform pxTransform(PX_REST_VEC(transform->position));
            const px::PxBoxGeometry pxGeometry(event.hx / 2.0f, event.hy / 2.0f, event.hz / 2.0f);

            px::PxRigidDynamic *dynamic = PxCreateDynamic(*gPhysics, pxTransform, pxGeometry, *gMaterial, 1.0f);
            dynamic->setAngularDamping(0.5f);
            dynamic->setLinearVelocity(px::PxVec3(0, 0, 0));
            px::PxRigidBodyExt::updateMassAndInertia(*dynamic, 3000.0f);

            gScene->addActor(*dynamic);

            entity.assign<components::Physic>(dynamic);
        }

        void Physic::receive(const events::PhysicForce &event) {
            ex::Entity entity = event.entity;

            ex::ComponentHandle<components::Physic> physic = entity.component<components::Physic>();

            px::PxVec3 dir(event.direction.x, event.direction.y, event.direction.z);

            auto *actor = static_cast<px::PxRigidDynamic *>(physic->actor);

//                actor->setLinearVelocity(dir * 1.0f);
            actor->addForce(dir, px::PxForceMode::eVELOCITY_CHANGE);
        }

        void Physic::receive(const events::SetupControlled &event) {
            ex::Entity entity = event.entity;

            auto model = components::get<components::Model>(entity);
            auto transform = components::get<components::Transform>(entity);

            Math::Box3 boundingBox = model->getBoundingBox();
            vec3 scaledRadius = boundingBox.radius * transform->getScale();

            px::PxCapsuleControllerDesc cDesc;

            cDesc.height = 1.0f;
            cDesc.radius = scaledRadius.y;
            cDesc.upDirection = px::PxVec3(0.0f, 1.0f, 0.0f);
            cDesc.material = pxMaterials["default"];
            cDesc.position = px::PxExtendedVec3(0.0f, cDesc.height, 0.0f);
            cDesc.slopeLimit = 0.1f;
            cDesc.contactOffset = 0.01f;
            cDesc.stepOffset = 0.1f;
            cDesc.scaleCoeff = 0.9f;
            cDesc.climbingMode = px::PxCapsuleClimbingMode::eEASY;
            cDesc.invisibleWallHeight = 0.0f;
            cDesc.maxJumpHeight = 0.0f;
//                cDesc.reportCallback		= this;

            auto *mController = static_cast<px::PxCapsuleController *>(gControllerManager->createController(cDesc));

//                entity.assign<components::Controlled>(mController);
            entity.assign<components::UserControl>(mController);
        }

        void Physic::receive(const events::KeyPress &event) {
            if (event.key == InputHandler::KEY_W) {
                controlDir.z = event.action != 0 ? 0.15f : 0.0f;
            }

            if (event.key == InputHandler::KEY_S) {
                controlDir.z = event.action != 0 ? -0.15f : 0.0f;
            }

            if (event.key == InputHandler::KEY_A) {
                controlDir.x = event.action != 0 ? -0.15f : 0.0f;
            }

            if (event.key == InputHandler::KEY_D) {
                controlDir.x = event.action != 0 ? 0.15f : 0.0f;
            }

            if (event.key == InputHandler::KEY_SPACE) {
                controlDir.y = event.action != 0 ? 0.15f : 0.0f;
            }

            if (event.key == InputHandler::KEY_C) {
                controlDir.y = event.action != 0 ? -0.15f : 0.0f;
            }
        }

        void Physic::makeStatic(ex::Entity &entity) {
            auto transform = entity.component<components::Transform>();
            px::PxMaterial *material = findMaterial("default");

            px::PxRigidStatic* actor = gPhysics->createRigidStatic(px::PxTransform(PX_REST_VEC(transform->position)));
            px::PxRigidActorExt::createExclusiveShape(*actor, px::PxBoxGeometry(1.0f, 1.0f, 1.0f), *material);

            gScene->addActor(*actor);
            entity.assign<c::Physic>(actor);
        }

        void Physic::makeDynamic(ex::Entity &entity) {
            auto transform = entity.component<components::Transform>();
            px::PxMaterial *material = findMaterial("default");

            px::PxRigidDynamic* actor = gPhysics->createRigidDynamic(px::PxTransform(PX_REST_VEC(transform->position)));

            // @todo: iterate over all meshes
            px::PxRigidActorExt::createExclusiveShape(*actor, px::PxBoxGeometry(1.f, 1.0f, 1.0f), *material);

            actor->setAngularVelocity(px::PxVec3(0.0f, 0.0f, 0.0f));
            actor->setAngularDamping(0.f);

            gScene->addActor(*actor);
            entity.assign<c::Physic>(actor);
        }

        px::PxMaterial *Physic::findMaterial(const std::string &name) {
            return pxMaterials.find(name)->second;
        }

        void Physic::onContact(const px::PxContactPairHeader &pairHeader, const px::PxContactPair *pairs, px::PxU32 nbPairs) {
            contactedActors.push({pairHeader.actors[0], pairHeader.actors[1]});
        }

        void Physic::onTrigger(px::PxTriggerPair *pairs, px::PxU32 count) {
            console::info("onTrigger");
        }

        void Physic::onConstraintBreak(px::PxConstraintInfo *, px::PxU32) {
            console::info("onConstraintBreak");
        }

        void Physic::onWake(px::PxActor **, px::PxU32) {
            console::info("onWake");
        }

        void Physic::onSleep(px::PxActor **, px::PxU32) {
            console::info("onSleep");
        }

        void Physic::onAdvance(const px::PxRigidBody *const *, const px::PxTransform *, const px::PxU32) {
            console::info("onAdvance");
        }

        HeightMap *Physic::createHeightMap(const std::shared_ptr<ImageData> &image) {
            ImageData::RawData *data = image->get();
            int width = image->getWidth();
            int height = image->getHeight();
            int step = image->getStride();

            auto *heightmap = new HeightMap(width, height);

            for (int i = 0; i < heightmap->size; i++) {
                int r = data[(i*step)+0];
                int g = data[(i*step)+1];
                int b = data[(i*step)+2];

                heightmap->values[i] = static_cast<px::PxI16>((r + g + b) / 3);
            }

            return heightmap;
        }

        px::PxRigidStatic *Physic::generateTerrain(const HeightMap &heightmap, const px::PxReal &width, const px::PxReal &height) {
            auto samples = new px::PxHeightFieldSample[heightmap.size];

            for (size_t i = 0; i < heightmap.size; i ++)
            {
                samples[i].height = heightmap.values[i];
                samples[i].materialIndex0 = 2;
                samples[i].materialIndex1 = 3;
            }

            px::PxHeightFieldDesc heightFieldDesc;
            heightFieldDesc.nbColumns = heightmap.cols;
            heightFieldDesc.nbRows = heightmap.rows;
            heightFieldDesc.thickness = -10;
            heightFieldDesc.convexEdgeThreshold = 3;
            heightFieldDesc.samples.data = samples;
            heightFieldDesc.samples.stride = sizeof(px::PxHeightFieldSample);

            px::PxHeightField* pHeightField = cooking->createHeightField(heightFieldDesc, gPhysics->getPhysicsInsertionCallback());

            if (!pHeightField) {
                console::warn("failed create highfield");
                return nullptr;
            }

            const px::PxReal heightScale = 0.01f;
            const px::PxReal hfRowsScale = static_cast<px::PxReal>(height) / static_cast<px::PxReal>(heightmap.rows);
            const px::PxReal hfColumnScale = static_cast<px::PxReal>(width) / static_cast<px::PxReal>(heightmap.cols);

            px::PxTransform pose = px::PxTransform(px::PxIdentity);
            pose.p.x = -(static_cast<px::PxReal>(heightmap.rows) * hfRowsScale / 2.0f);
            pose.p.z = -(static_cast<px::PxReal>(heightmap.cols) * hfColumnScale / 2.0f);
            px::PxRigidStatic* hfActor = gPhysics->createRigidStatic(pose);

            px::PxHeightFieldGeometry hfGeom(pHeightField, px::PxMeshGeometryFlags(), heightScale, hfRowsScale, hfColumnScale);
            px::PxShape* hfShape = px::PxRigidActorExt::createExclusiveShape(*hfActor, hfGeom, *findMaterial("default"));
            if(!hfShape) {
                console::warn("creating heightfield shape failed");
                return nullptr;
            }

            delete[] samples;

            gScene->addActor(*hfActor);

            return hfActor;
        }

        Mesh *Physic::generateTerrainMesh(px::PxRigidStatic *actor, const HeightMap &heightmap) {
            px::PxShape *shape;
            actor->getShapes(&shape, 1);

            physx::PxHeightFieldGeometry hfg;
            shape->getHeightFieldGeometry(hfg);

            Mesh *mesh = Mesh::Create();
            mesh->geometry.allocVertices(heightmap.size);
            mesh->material->setDiffuse(0.0f, 1.0f, 0.0f);

            const float textureMultiplier = 4.0f;

            for(px::PxU32 y = 0; y < heightmap.rows; y++)
            {
                for(px::PxU32 x = 0; x < heightmap.cols; x++)
                {
                    Vertex vertex;
                    vertex.Position = vec3(
                        (px::PxReal(y)) * hfg.rowScale,
                        heightmap.values[x + y * heightmap.cols] * hfg.heightScale,
                        px::PxReal(x) * hfg.columnScale
                    );

                    vertex.Normal = vec3(0.0f, 1.0f, 0.0f);
                    vertex.TexCoords = vec2(
                            (static_cast<float>(x) / heightmap.cols) * textureMultiplier,
                            (1.0f - (static_cast<float>(y) / heightmap.rows)) * textureMultiplier
                    );

                    mesh->geometry.addVertex(vertex);
                }
            }

            px::PxU32 numTris = 0;
            for(px::PxU32 j = 0; j < heightmap.rows - 1; j++)
            {
                for(px::PxU32 i = 0; i < heightmap.cols - 1; i++)
                {
                    mesh->geometry.addFace(
                        (i + j * (heightmap.cols)),
                        (i + (j+1) * (heightmap.cols)),
                        (i + 1 + j * (heightmap.cols))
                    );

                    mesh->geometry.addFace(
                        i + (j + 1) * (heightmap.cols),
                        i + 1 + (j + 1) * (heightmap.cols),
                        i + 1 + j * (heightmap.cols)
                    );


                    numTris+=2;
                }
            }

            return mesh;
        }
    }
}