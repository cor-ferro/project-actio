#include "physic.h"
#include "../world.h"

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
                        if (!physic.dynamic->isSleeping()) {
                            px::PxTransform newTransform = physic.dynamic->getGlobalPose();

                            transform.setPosition(newTransform.p.x, newTransform.p.y, newTransform.p.z);
                            transform.setQuaternion(newTransform.q.x, newTransform.q.y, newTransform.q.z,
                                                    newTransform.q.w);
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

                if (!character.isJump && pxPos.y > 0.0f) {
                    character.jump -= (gravity.y) * elapsedTime;
                    character.motion.y -= 0.1f * character.jump;
                }

                transform.setPosition(
                        static_cast<float>(pxPos.x),
                        static_cast<float>(pxPos.y),
                        static_cast<float>(pxPos.z)
                );
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
//                physic->dynamic->setLinearVelocity(dir * 1.0f);
            physic->dynamic->addForce(dir, px::PxForceMode::eVELOCITY_CHANGE);
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
    }
}