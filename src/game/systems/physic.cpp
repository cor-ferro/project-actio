#include "physic.h"
#include <glm/gtc/random.hpp>

namespace game {
    namespace systems {
        Physic::Physic(Context& context) : systems::BaseSystem(context) {

        }

        Physic::~Physic() {
            if (debugLinesEntity.valid()) debugLinesEntity.destroy();
            if (debugTrianglesEntity.valid()) debugTrianglesEntity.destroy();
            if (debugPointsEntity.valid()) debugPointsEntity.destroy();
        }

        void Physic::configure(ex::EntityManager& entities, entityx::EventManager& events) {
            events.subscribe<ex::EntityCreatedEvent>(*this);
            events.subscribe<ex::EntityDestroyedEvent>(*this);
            events.subscribe<events::ObjectCreate>(*this);
            events.subscribe<events::PhysicForce>(*this);
            events.subscribe<events::MakeControlled>(*this);
            events.subscribe<events::MakeUnControlled>(*this);

//            {
////                debugLinesObject = world->createObject();
//                debugLinesObject = context->createObject();
//
//                std::shared_ptr<Mesh> mesh = Mesh::Create();
//
//                GeometryBuilder::Lines(mesh->geometry, {});
//                mesh->geometry.setType(Geometry::Geometry_Dynamic);
//                mesh->material->setDiffuse(vec3(0.0f, 1.0f, 0.0f));
//
//                mesh->setPrimitiveType(Mesh_Primitive_Line);
//
//                world->setObjectMesh(debugLinesObject, mesh);
//            }

//            {
//                debugTrianglesObject = context->createObject();
//
//                std::shared_ptr<Mesh> mesh = Mesh::Create();
//
//                mesh->geometry.setType(Geometry::Geometry_Dynamic);
//                mesh->material->setDiffuse(vec3(0.0f, 0.0f, 1.0f));
//
//                mesh->setPrimitiveType(Mesh_Primitive_Triangle);
//
//                world->setObjectMesh(debugTrianglesObject, mesh);
//            }

//            {
//                debugPointsObject = context->createObject();
//
//                std::shared_ptr<Mesh> mesh = Mesh::Create();
//
//                mesh->geometry.setType(Geometry::Geometry_Dynamic);
//                mesh->material->setDiffuse(vec3(1.0f, 0.0f, 0.0f));
//
//                mesh->setPrimitiveType(Mesh_Primitive_Point);
//
//                world->setObjectMesh(debugPointsObject, mesh);
//            }
        }

        void Physic::start(ex::EntityManager& es, ex::EventManager& events, ex::TimeDelta dt) {
            systems::BaseSystem::start(es, events, dt);

            debugLinesEntity = m_context.createObject(es);
            debugTrianglesEntity = m_context.createObject(es);
            debugPointsEntity = m_context.createObject(es);

            MeshHandle debugLinesMesh = m_context.meshes().create();
            MeshHandle debugTrianglesMesh = m_context.meshes().create();
            MeshHandle debugPointsMesh = m_context.meshes().create();

            debugLinesEntity.assign<components::Mesh>(debugLinesMesh);
            debugLinesEntity.assign<components::Renderable>();

            debugTrianglesEntity.assign<components::Mesh>(debugTrianglesMesh);
            debugTrianglesEntity.assign<components::Renderable>();

            debugPointsEntity.assign<components::Mesh>(debugPointsMesh);
            debugPointsEntity.assign<components::Renderable>();

            events.emit<events::ObjectCreate>(debugLinesEntity);
            events.emit<events::ObjectCreate>(debugTrianglesEntity);
            events.emit<events::ObjectCreate>(debugPointsEntity);
        }

        void Physic::update(ex::EntityManager& es, ex::EventManager& events, ex::TimeDelta dt) {
            PX_UNUSED(false);

            auto elapsedTime = static_cast<px::PxReal>(dt / 1000.0);

            m_context.physic().update(elapsedTime);

            while (!contactedActors.empty()) {
                const std::pair<px::PxActor *, px::PxActor *>& pair = contactedActors.top();

                events.emit<events::PhysicContact>(pair.first, pair.second);

                contactedActors.pop();
            }

            es.each<c::Character, c::UserControl, c::Transform>([&elapsedTime, this](
                    ex::Entity entity,
                    c::Character& character,
                    c::UserControl& userControl,
                    c::Transform& transform
            ) {
                const px::PxVec3 gravity = m_context.physic().getSceneGravity();

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

                const px::PxExtendedVec3& pxPos = userControl.controller->getFootPosition();

                if (!character.isJump && pxPos.y > 0.0) {
                    character.jump -= (gravity.y) * elapsedTime;
                    character.motion.y -= 0.1f * character.jump;
                }

                transform.setPosition(px::toVec3(pxPos));
            });

            if (isDisplayDebug) {
                updateDebugBuffer(events);
            }
        }

        void Physic::updateDebugBuffer(ex::EventManager& events) {
            const px::PxRenderBuffer& rb = m_context.physic().getScene()->getRenderBuffer();

            MeshHandle& linesMesh = debugLinesEntity.component<components::Mesh>()->get();
            MeshHandle& trianglesMesh = debugTrianglesEntity.component<components::Mesh>()->get();
            MeshHandle& pointsMesh = debugPointsEntity.component<components::Mesh>()->get();

            const px::PxU32 countLines = rb.getNbLines();
            const px::PxU32 countTriangles = rb.getNbTriangles();
            const px::PxU32 countPoints = rb.getNbPoints();

            {
                if (countLines * 2 != linesMesh->geometry.getCountVertices()) {
                    std::vector<vec3> vertices;
                    vertices.reserve(countLines * 2);

                    for (px::PxU32 i = 0; i < countLines; i++) {
                        const px::PxDebugLine& line = rb.getLines()[i];

                        vertices.push_back(vec3(line.pos0.x, line.pos0.y, line.pos0.z));
                        vertices.push_back(vec3(line.pos1.x, line.pos1.y, line.pos1.z));
                    }

                    linesMesh->geometry.setVertices(vertices);
                    linesMesh->geometry.setNeedUpdateVertices(true);
                } else {
                    GeometryVertices *vertices = linesMesh->geometry.getVertices();

                    for (px::PxU32 i = 0; i < countLines; i++) {
                        const px::PxDebugLine& line = rb.getLines()[i];

                        Vertex& vertex = vertices->at(i * 2);
                        vertex.Position = vec3(line.pos0.x, line.pos0.y, line.pos0.z);

                        Vertex& vertex2 = vertices->at((i * 2) + 1);
                        vertex2.Position = vec3(line.pos1.x, line.pos1.y, line.pos1.z);
                    }
                }
            }

            {
                if (countTriangles * 3 != trianglesMesh->geometry.getCountVertices()) {
                    std::vector<vec3> vertices;
                    vertices.reserve(countTriangles * 3);

                    for (px::PxU32 i = 0; i < countTriangles; i++) {
                        const px::PxDebugTriangle& triangle = rb.getTriangles()[i];

                        vertices.push_back(vec3(triangle.pos0.x, triangle.pos0.y, triangle.pos0.z));
                        vertices.push_back(vec3(triangle.pos1.x, triangle.pos1.y, triangle.pos1.z));
                        vertices.push_back(vec3(triangle.pos2.x, triangle.pos2.y, triangle.pos2.z));
                    }

                    trianglesMesh->geometry.setVertices(vertices);
                    trianglesMesh->geometry.setNeedUpdateVertices(true);
                } else {
                    GeometryVertices *vertices = trianglesMesh->geometry.getVertices();

                    for (px::PxU32 i = 0; i < countTriangles; i++) {
                        const px::PxDebugTriangle& triangle = rb.getTriangles()[i];

                        Vertex& vertex = vertices->at(i * 3);
                        vertex.Position = vec3(triangle.pos0.x, triangle.pos0.y, triangle.pos0.z);

                        Vertex& vertex2 = vertices->at((i * 3) + 1);
                        vertex2.Position = vec3(triangle.pos1.x, triangle.pos1.y, triangle.pos1.z);

                        Vertex& vertex3 = vertices->at((i * 3) + 2);
                        vertex3.Position = vec3(triangle.pos2.x, triangle.pos2.y, triangle.pos2.z);
                    }
                }
            }

            {
                if (countPoints != pointsMesh->geometry.getCountVertices()) {
                    std::vector<vec3> vertices;
                    vertices.reserve(countPoints);

                    for (px::PxU32 i = 0; i < countPoints; i++) {
                        const px::PxDebugPoint& point = rb.getPoints()[i];

                        vertices.push_back(vec3(point.pos.x, point.pos.y, point.pos.z));
                    }

                    pointsMesh->geometry.setVertices(vertices);
                    pointsMesh->geometry.setNeedUpdateVertices(true);
                } else {
                    GeometryVertices *vertices = pointsMesh->geometry.getVertices();

                    for (px::PxU32 i = 0; i < countPoints; i++) {
                        const px::PxDebugPoint& point = rb.getPoints()[i];

                        Vertex& vertex = vertices->at(i);
                        vertex.Position = vec3(point.pos.x, point.pos.y, point.pos.z);
                    }
                }
            }

            events.emit<events::RenderUpdateMesh>(debugLinesEntity);
            events.emit<events::RenderUpdateMesh>(debugTrianglesEntity);
            events.emit<events::RenderUpdateMesh>(debugPointsEntity);
        }

        void Physic::receive(const ex::EntityCreatedEvent& event) {
//                const Entity& e = event.entity;
//                event.entity.assign<c::Controlled>(m_controllerManager);
        }

        void Physic::receive(const ex::EntityDestroyedEvent& event) {
//                event.entity.assign<c::Controlled>(m_controllerManager);
        }

        void Physic::receive(const events::ObjectCreate& event) {
            if (event.entity.has_component<components::PhysicActor>()) {
                ex::Entity entity = event.entity;

                auto physic = entity.component<components::PhysicActor>();

//                if (physic->isDynamic()) {
//                    auto actor = physic->getDynamicActor();
//
//                    actor->setAngularVelocity(px::PxVec3(0.0f, 0.0f, 0.0f));
//                    actor->setAngularDamping(0.0f);
//                    px::PxRigidBodyExt::updateMassAndInertia(*actor, 100.0f);
//                }

                m_context.physic().addToScene(physic->getActor());
            }
        }

        void Physic::receive(const events::PhysicForce& event) {
            ex::Entity entity = event.entity;

            m_context.physic().applyForce(entity, event.direction);
        }

        void Physic::receive(const events::MakeControlled& event) {
            ex::Entity entity = event.entity;

            float radius = 0.0f;

            if (entity.has_component<c::Mesh>()) {
                auto meshes = components::get<c::Mesh>(entity);
                auto transform = components::get<c::Transform>(entity);

                Math::Box3 boundingBox = meshes->getBoundingBox();
                vec3 scaledRadius = boundingBox.radius * transform->getScale();

                radius = scaledRadius.y;
            } else {
                radius = 1.0f;
            }

            px::PxCapsuleControllerDesc cDesc;

            cDesc.height = 2.0f;
            cDesc.radius = radius;
            cDesc.upDirection = px::PxVec3(0.0f, 1.0f, 0.0f);
            cDesc.material = m_context.physic().getMaterial();
            cDesc.position = px::PxExtendedVec3(0.0f, 1.0f + cDesc.height, 0.0f);
            cDesc.slopeLimit = 0.1f;
            cDesc.contactOffset = 0.01f;
            cDesc.stepOffset = 0.1f;
            cDesc.scaleCoeff = 1.0f;
            cDesc.climbingMode = px::PxCapsuleClimbingMode::eEASY;
            cDesc.invisibleWallHeight = 0.0f;
            cDesc.maxJumpHeight = 0.0f;
//            cDesc.reportCallback		= this;

            auto *mController = static_cast<px::PxCapsuleController *>(m_context.physic().getControllerManager()->createController(cDesc));

            entity.assign<c::UserControl>(mController);
        }

        void Physic::receive(const events::MakeUnControlled& event) {
            ex::Entity entity = event.entity;
            entity.remove<c::UserControl>();
        }

        HeightMap *Physic::createHeightMap(const std::shared_ptr<ImageData>& image) {
            ImageData::RawData *data = image->get();
            int width = image->getWidth();
            int height = image->getHeight();
            int step = image->getStride();

            auto *heightmap = new HeightMap(width, height);

            for (int i = 0; i < heightmap->size; i++) {
                int r = data[(i * step) + 0];
                int g = data[(i * step) + 1];
                int b = data[(i * step) + 2];

                heightmap->values[i] = static_cast<px::PxI16>((r + g + b) / 3);
            }

            return heightmap;
        }

        px::PxRigidStatic *
        Physic::generateTerrain(const HeightMap& heightmap, const px::PxReal& width, const px::PxReal& height) {
            auto samples = new px::PxHeightFieldSample[heightmap.size];

            for (size_t i = 0; i < heightmap.size; i++) {
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

            px::PxHeightField *pHeightField = m_context.physic().getCooking()->createHeightField(
                    heightFieldDesc,
                    m_context.physic().getPhysics()->getPhysicsInsertionCallback()
            );

            if (!pHeightField) {
                console::warn("failed create highfield");
                return nullptr;
            }

            const px::PxReal heightScale = 0.03f;
            const px::PxReal hfRowsScale = static_cast<px::PxReal>(height) / static_cast<px::PxReal>(heightmap.rows);
            const px::PxReal hfColumnScale = static_cast<px::PxReal>(width) / static_cast<px::PxReal>(heightmap.cols);

            px::PxTransform pose = px::PxTransform(px::PxIdentity);
            pose.p.x = -(static_cast<px::PxReal>(heightmap.rows) * hfRowsScale / 2.0f);
            pose.p.z = -(static_cast<px::PxReal>(heightmap.cols) * hfColumnScale / 2.0f);
            px::PxRigidStatic *hfActor = m_context.physic().getPhysics()->createRigidStatic(pose);

            px::PxHeightFieldGeometry hfGeom(pHeightField, px::PxMeshGeometryFlags(), heightScale, hfRowsScale,
                                             hfColumnScale);
            px::PxShape *hfShape = px::PxRigidActorExt::createExclusiveShape(*hfActor, hfGeom,
                                                                             *m_context.physic().getMaterial());
            if (!hfShape) {
                console::warn("creating heightfield shape failed");
                return nullptr;
            }

            delete[] samples;

            m_context.physic().addToScene(hfActor);

            return hfActor;
        }

        std::shared_ptr<Mesh> Physic::generateTerrainMesh(px::PxRigidStatic *actor, const HeightMap& heightmap) {
            px::PxShape *shape;
            actor->getShapes(&shape, 1);

            physx::PxHeightFieldGeometry hfg;
            shape->getHeightFieldGeometry(hfg);

            std::shared_ptr<Mesh> mesh = m_context.meshes().create();
            mesh->geometry.allocVertices(heightmap.size);
            mesh->material->setDiffuse(0.0f, 1.0f, 0.0f);

            const float textureMultiplier = 16.0f;

            for (px::PxU32 y = 0; y < heightmap.rows; y++) {
                for (px::PxU32 x = 0; x < heightmap.cols; x++) {
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
            for (px::PxU32 j = 0; j < heightmap.rows - 1; j++) {
                for (px::PxU32 i = 0; i < heightmap.cols - 1; i++) {
                    mesh->geometry.addFace(
                            (i + j * (heightmap.cols)),
                            (i + (j + 1) * (heightmap.cols)),
                            (i + 1 + j * (heightmap.cols))
                    );

                    mesh->geometry.addFace(
                            i + (j + 1) * (heightmap.cols),
                            i + 1 + (j + 1) * (heightmap.cols),
                            i + 1 + j * (heightmap.cols)
                    );


                    numTris += 2;
                }
            }

            return mesh;
        }

        void Physic::computeBoundingBox(game::WorldObject *object) {
            if (object->hasComponent<c::Mesh>() && object->hasComponent<c::PhysicActor>()) {
                auto meshes = object->getComponent<c::Mesh>();
                auto physic = object->getComponent<c::PhysicActor>();

                const Math::Box3 box = meshes->getBoundingBox();
                float height = meshes->height();

                px::PxBoxGeometry geometry;
                geometry.halfExtents = px::PxVec3(
                        box.radius.x,
                        box.radius.y,
                        box.radius.z
                );

                px::PxMaterial *material = m_context.physic().getMaterial("default");
                px::PxRigidActorExt::createExclusiveShape(*physic->getActor(), geometry, *material);
            }
        }

    }
}