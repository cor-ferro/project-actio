//
// Created by demitriy on 3/24/18.
//

#ifndef ACTIO_PHYSIC_H
#define ACTIO_PHYSIC_H

#include <entityx/entityx/Entity.h>
#include <entityx/entityx/System.h>
#include "PxPhysics.h"
#include "PxScene.h"
#include "PxRigidDynamic.h"
#include "PxShape.h"
#include "PxPhysicsAPI.h"
#include "../components/physic.h"
#include "../components/transform.h"
#include "../../lib/console.h"
#include "../components/controlled.h"
#include "../events/physic_create.h"
#include "../events/physic_force.h"
#include "../events/setup_controlled.h"
#include "../events/key_press.h"
#include "../events/render_setup_mesh.h"
#include "../events/render_update_mesh.h"
#include "../../core/geometry_primitive.h"
#include "../components/base.h"
#include "../components/renderable.h"

namespace game {
    namespace systems {
        using namespace entityx;
        using namespace physx;

        namespace ex = entityx;

        class Physic : public entityx::System<Physic>, public Receiver<Physic> {
        public:
            enum SceneParam {
                Scene_Gravity
            };

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
                gScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
                gScene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 1.0f);
                gScene->setVisualizationParameter(PxVisualizationParameter::eWORLD_AXES, 1.0f);
                gScene->setVisualizationParameter(PxVisualizationParameter::eBODY_AXES, 1.0f);
                gScene->setVisualizationParameter(PxVisualizationParameter::eBODY_MASS_AXES, 1.0f);
                gScene->setVisualizationParameter(PxVisualizationParameter::eBODY_LIN_VELOCITY, 1.0f);
                gScene->setVisualizationParameter(PxVisualizationParameter::eBODY_ANG_VELOCITY, 1.0f);
                gScene->setVisualizationParameter(PxVisualizationParameter::eCONTACT_POINT, 1.0f);
                gScene->setVisualizationParameter(PxVisualizationParameter::eCONTACT_NORMAL, 1.0f);
                gScene->setVisualizationParameter(PxVisualizationParameter::eCONTACT_ERROR, 1.0f);
                gScene->setVisualizationParameter(PxVisualizationParameter::eCONTACT_FORCE, 1.0f);
                gScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_AABBS, 1.0f);
//                gScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
                gScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_AXES, 1.0f);
                gScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_EDGES, 1.0f);
//                gScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_STATIC, 1.0f);
//                gScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_DYNAMIC, 1.0f);
                gScene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LIMITS, 1.0f);
                gScene->setVisualizationParameter(PxVisualizationParameter::eCULL_BOX, 1.0f);

                gControllerManager = PxCreateControllerManager(*gScene);

                pxMaterials.insert({"default", gPhysics->createMaterial(0.5f, 0.5f, 0.6f)});

                PxRigidStatic *groundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *(pxMaterials["default"]));
                gScene->addActor(*groundPlane);

                controlDir = PxVec3(0.0f);

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

            void configure(EntityManager &entities, entityx::EventManager &event_manager) {
                event_manager.subscribe<EntityCreatedEvent>(*this);
                event_manager.subscribe<EntityDestroyedEvent>(*this);
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
                    mesh->material.setDiffuse(vec3(0.0f, 1.0f, 0.0f));

                    mesh->setDrawType(Mesh_Draw_Line);

                    debugLinesEntity.assign<components::Model>(mesh);
                    debugLinesEntity.assign<components::Transform>(vec3(0.0f));
                }

                {
                    debugTrianglesEntity = entities.create();

                    Mesh *mesh = Mesh::Create();

                    mesh->geometry.setType(Geometry::Geometry_Dynamic);
                    mesh->material.setDiffuse(vec3(0.0f, 0.0f, 1.0f));

                    mesh->setDrawType(Mesh_Draw_Triangle);

                    debugTrianglesEntity.assign<components::Model>(mesh);
                    debugTrianglesEntity.assign<components::Transform>(vec3(0.0f));
                }

                {
                    debugPointsEntity = entities.create();

                    Mesh *mesh = Mesh::Create();

                    mesh->geometry.setType(Geometry::Geometry_Dynamic);
                    mesh->material.setDiffuse(vec3(1.0f, 0.0f, 0.0f));

                    mesh->setDrawType(Mesh_Draw_Point);

                    debugPointsEntity.assign<components::Model>(mesh);
                    debugPointsEntity.assign<components::Transform>(vec3(0.0f));
                }
            }

            void postConfigure(entityx::EventManager &event_manager) {
                auto modelLines = components::get<components::Model>(debugLinesEntity);
                auto modelTriangles = components::get<components::Model>(debugTrianglesEntity);
                auto modelPoints = components::get<components::Model>(debugPointsEntity);

                event_manager.emit<events::RenderSetupMesh>(debugLinesEntity, modelLines->getMesh());
                event_manager.emit<events::RenderSetupMesh>(debugTrianglesEntity, modelTriangles->getMesh());
                event_manager.emit<events::RenderSetupMesh>(debugPointsEntity, modelPoints->getMesh());
            }

            void destroy() {

            }

            void update(EntityManager &es, EventManager &events, TimeDelta dt) override {
                PX_UNUSED(false);

                PxF32 elapsedTime = static_cast<PxReal>(dt / 1000.0);

                gScene->simulate(elapsedTime);
                gScene->fetchResults(true);

                es.each<components::Physic, components::Transform>(
                        [](
                                Entity entity,
                                components::Physic &physic,
                                components::Transform &transform
                        ) {
                            if (!physic.dynamic->isSleeping()) {
                                PxTransform newTransform = physic.dynamic->getGlobalPose();

                                transform.setPosition(newTransform.p.x, newTransform.p.y, newTransform.p.z);
                                transform.setQuaternion(newTransform.q.x, newTransform.q.y, newTransform.q.z, newTransform.q.w);
                            }
                        }
                );

                PxVec3 controlDir = this->controlDir;
                PxVec3 gravity = gScene->getGravity();

                es.each<components::Controlled, components::Transform, components::Skin>(
                        [&elapsedTime, &controlDir, &gravity](
                                Entity entity,
                                components::Controlled &control,
                                components::Transform &transform,
                                components::Skin &skin
                        ) {
                            if (control.isJump) {
                                control.jumpForce += (gravity.y) * elapsedTime;
                                control.disp.y+= 0.1f * control.jumpForce;
                            }

                            const PxControllerCollisionFlags flags = control.controller->move(
                                    PxVec3(control.disp.x, control.disp.y, control.disp.z), 0.01f, elapsedTime, PxControllerFilters()
                            );

                            if(flags & PxControllerCollisionFlag::eCOLLISION_DOWN) {
                                console::info("eCOLLISION_DOWN");
                                control.isJump = false;
                                control.jumpForce = 0.0f;
                                control.disp.y = 0.0f;
                            }

                            if(flags & PxControllerCollisionFlag::eCOLLISION_UP)
                                console::info("eCOLLISION_UP");
                            if(flags & PxControllerCollisionFlag::eCOLLISION_SIDES)
                                console::info("eCOLLISION_SIDES");

                            const PxExtendedVec3 &pxPos = control.controller->getFootPosition();

                            transform.setPosition(
                                    static_cast<float>(pxPos.x),
                                    static_cast<float>(pxPos.y),
                                    static_cast<float>(pxPos.z)
                            );
                        }
                );

                if (drawDebug) {
                    drawDebugBuffer(events);
                }
            }

            void add() {
                PxRigidDynamic *ball = createDynamic(PxTransform(PxVec3(0, 20, 0)), PxSphereGeometry(3),
                                                     PxVec3(0, -25, -5));
                PxRigidBodyExt::updateMassAndInertia(*ball, 1000.0f);
                gScene->addActor(*ball);
            }

            void setDrawDebug(bool value) {
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

            void drawDebugBuffer(EventManager &events) {
                const PxRenderBuffer &rb = gScene->getRenderBuffer();

                auto linesModel = components::get<components::Model>(debugLinesEntity);
                Mesh *linesMesh = linesModel->getMesh();

                auto trianglesModel = components::get<components::Model>(debugTrianglesEntity);
                Mesh *trianglesMesh = trianglesModel->getMesh();

                auto pointsModel = components::get<components::Model>(debugPointsEntity);
                Mesh *pointsMesh = pointsModel->getMesh();

                const PxU32 countLines = rb.getNbLines();
                const PxU32 countTriangles = rb.getNbTriangles();
                const PxU32 countPoints = rb.getNbPoints();

                {
                    if (countLines * 2 != linesMesh->geometry.getCountVertices()) {
                        std::vector<vec3> vertices;
                        vertices.reserve(countLines * 2);

                        for (PxU32 i = 0; i < countLines; i++) {
                            const PxDebugLine &line = rb.getLines()[i];

                            vertices.push_back(vec3(line.pos0.x, line.pos0.y, line.pos0.z));
                            vertices.push_back(vec3(line.pos1.x, line.pos1.y, line.pos1.z));
                        }

                        linesMesh->geometry.setVertices(vertices);
                        linesMesh->geometry.setNeedUpdateVertices(true);
                    } else {
                        GeometryVertices *vertices = linesMesh->geometry.getVertices();

                        for (PxU32 i = 0; i < countLines; i++) {
                            const PxDebugLine &line = rb.getLines()[i];

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

                        for (PxU32 i = 0; i < countTriangles; i++) {
                            const PxDebugTriangle &triangle = rb.getTriangles()[i];

                            vertices.push_back(vec3(triangle.pos0.x, triangle.pos0.y, triangle.pos0.z));
                            vertices.push_back(vec3(triangle.pos1.x, triangle.pos1.y, triangle.pos1.z));
                            vertices.push_back(vec3(triangle.pos2.x, triangle.pos2.y, triangle.pos2.z));
                        }

                        trianglesMesh->geometry.setVertices(vertices);
                        trianglesMesh->geometry.setNeedUpdateVertices(true);
                    } else {
                        GeometryVertices *vertices = trianglesMesh->geometry.getVertices();

                        for (PxU32 i = 0; i < countTriangles; i++) {
                            const PxDebugTriangle &triangle = rb.getTriangles()[i];

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

                        for (PxU32 i = 0; i < countPoints; i++) {
                            const PxDebugPoint &point = rb.getPoints()[i];

                            vertices.push_back(vec3(point.pos.x, point.pos.y, point.pos.z));
                        }

                        pointsMesh->geometry.setVertices(vertices);
                        pointsMesh->geometry.setNeedUpdateVertices(true);
                    } else {
                        GeometryVertices *vertices = pointsMesh->geometry.getVertices();

                        for (PxU32 i = 0; i < countPoints; i++) {
                            const PxDebugPoint &point = rb.getPoints()[i];

                            Vertex &vertex = vertices->at(i);
                            vertex.Position = vec3(point.pos.x, point.pos.y, point.pos.z);
                        }
                    }
                }

                events.emit<events::RenderUpdateMesh>(debugLinesEntity, linesMesh);
                events.emit<events::RenderUpdateMesh>(debugTrianglesEntity, trianglesMesh);
                events.emit<events::RenderUpdateMesh>(debugPointsEntity, pointsMesh);
            }

            void setSceneGravity(vec3 value) {
                gScene->setGravity(PxVec3(value.x, value.y, value.z));
            };

            PxRigidDynamic *createDynamic(const PxTransform &t, const PxGeometry &geometry, const PxVec3 &velocity) {
                PxMaterial *gMaterial = pxMaterials.find("default")->second;

                PxRigidDynamic *dynamic = PxCreateDynamic(*gPhysics, t, geometry, *gMaterial, 10.0f);
                dynamic->setAngularDamping(0.5f);
                dynamic->setLinearVelocity(velocity);

                return dynamic;
            }

            void receive(const EntityCreatedEvent &event) {
//                const Entity& e = event.entity;
//                event.entity.assign<components::Controlled>(gControllerManager);
            }

            void receive(const EntityDestroyedEvent &event) {
//                event.entity.assign<components::Controlled>(gControllerManager);
            }

            void receive(const events::PhysicCreateSphere &event) {
                ex::Entity entity = event.entity;

                ex::ComponentHandle<components::Transform> transform = entity.component<components::Transform>();

                PxMaterial *gMaterial = pxMaterials.find("default")->second;
                const PxTransform pxTransform(
                        PxVec3(transform->position.x, transform->position.y, transform->position.z));
                const PxSphereGeometry pxGeometry(event.radius);

                PxRigidDynamic *dynamic = PxCreateDynamic(*gPhysics, pxTransform, pxGeometry, *gMaterial, 100.0f);
                dynamic->setAngularDamping(10.5f);
                dynamic->setLinearVelocity(PxVec3(0, 0, 0));
                PxRigidBodyExt::updateMassAndInertia(*dynamic, 1000.0f);

                gScene->addActor(*dynamic);

                entity.assign<components::Physic>(dynamic);
            }

            void receive(const events::PhysicCreateBox &event) {
                ex::Entity entity = event.entity;

                ex::ComponentHandle<components::Transform> transform = entity.component<components::Transform>();

                PxMaterial *gMaterial = pxMaterials.find("default")->second;
                const PxTransform pxTransform(
                        PxVec3(transform->position.x, transform->position.y, transform->position.z));
                const PxBoxGeometry pxGeometry(event.hx / 2.0f, event.hy / 2.0f, event.hz / 2.0f);

                PxRigidDynamic *dynamic = PxCreateDynamic(*gPhysics, pxTransform, pxGeometry, *gMaterial, 1.0f);
                dynamic->setAngularDamping(0.5f);
                dynamic->setLinearVelocity(PxVec3(0, 0, 0));
                PxRigidBodyExt::updateMassAndInertia(*dynamic, 3000.0f);

                gScene->addActor(*dynamic);

                entity.assign<components::Physic>(dynamic);
            }

            void receive(const events::PhysicForce &event) {
                ex::Entity entity = event.entity;

                ex::ComponentHandle<components::Physic> physic = entity.component<components::Physic>();

                PxVec3 dir(event.direction.x, event.direction.y, event.direction.z);
                physic->dynamic->addForce(dir, PxForceMode::eVELOCITY_CHANGE);
            }

            void receive(const events::SetupControlled &event) {
                ex::Entity entity = event.entity;

                PxCapsuleControllerDesc cDesc;

                cDesc.height = 3.5f;
                cDesc.radius = 0.5f;
                cDesc.upDirection = PxVec3(0.0f, 1.0f, 0.0f);
                cDesc.material = pxMaterials["default"];
                cDesc.position = PxExtendedVec3(0.0f, cDesc.height / 2.0f, 0.0f);
                cDesc.slopeLimit = 0.1f;
                cDesc.contactOffset = 0.01f;
                cDesc.stepOffset = 0.1f;
                cDesc.scaleCoeff = 0.9f;
                cDesc.climbingMode = PxCapsuleClimbingMode::eEASY;
                cDesc.invisibleWallHeight = 0.0f;
                cDesc.maxJumpHeight = 0.0f;
//                cDesc.reportCallback		= this;

                auto *mController = static_cast<PxCapsuleController *>(gControllerManager->createController(cDesc));

                entity.assign<components::Controlled>(mController);
            }

            void receive(const events::KeyPress &event) {
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

        private:
            PxDefaultAllocator gAllocator;
            PxDefaultErrorCallback gErrorCallback;
            PxFoundation *gFoundation = nullptr;
            PxPhysics *gPhysics = nullptr;
            PxPvd *gPvd = nullptr;
            PxDefaultCpuDispatcher *cpuDispatcher = nullptr;
            PxGpuDispatcher *gpuDispatcher = nullptr;
//            PxCudaContextManager* cudaContextManager = nullptr;
            PxScene *gScene = nullptr;
            PxControllerManager *gControllerManager = nullptr;
            std::unordered_map<std::string, PxMaterial *> pxMaterials;

            PxVec3 controlDir;
            entityx::Entity debugLinesEntity;
            entityx::Entity debugTrianglesEntity;
            entityx::Entity debugPointsEntity;

            bool drawDebug = false;
        };
    }
}

#endif //ACTIO_PHYSIC_H
