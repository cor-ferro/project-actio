#include "world.h"
#include <thread>
#include <entityx/entityx/Event.h>
#include <entityx/entityx/System.h>
#include "../../core/geometry_builder.h"
#include "../components/mesh.h"
#include "../components/renderable.h"
#include "../components/physic_actor.h"
#include "../components/camera.h"
#include "../components/target.h"
#include "../components/light_directional.h"
#include "../components/transform.h"
#include "../events/object_create.h"
#include "../context.h"

namespace game {
    systems::World::World(Context &context) : systems::BaseSystem(context) {

    }

    systems::World::~World() {

    }

    void systems::World::configure(ex::EntityManager &entities, entityx::EventManager &events) {
        events.subscribe<events::LoadStoryComplete>(*this);
    }

    void systems::World::start(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
        systems::BaseSystem::start(es, events, dt);

        // camera
        {
            auto camera = es.create();
            camera.assign<components::Camera>(45.0f, 1.41f, 1.0f, 500.0f);
            camera.assign<components::Transform>(vec3{0.0f, 0.0f, 0.0f});
            camera.assign<components::Target>(vec3{0.0f, 0.0f, 0.0f});
        }


        // light
        {
            auto globalLight = m_context.createObject(es);
            globalLight.assign<components::LightDirectional>(vec3{1.0f}, vec3{1.0f}, vec3{1.0f}, vec3{0.0f, 0.0f, -1.0f});
        }
    }

    void systems::World::Aircraft::Thruster::increaseRate(float value) {
        this->rate = glm::sin(this->rate + value);
    }

    void systems::World::Aircraft::Thruster::decreaseRate(float value) {
        this->rate = glm::sin(this->rate - value);
    }

    void systems::World::Aircraft::Thruster::decreaseRate() {
        this->rate = glm::sin(glm::tan(this->rate * 0.9f));
    }

    const float systems::World::Aircraft::Thruster::getPower() {
        return glm::max(1.0f, this->rate * this->maxPower/* * (1.0f - glm::sin(glm::pow(this->overheat, 3.0f)))*/);
    }

    const float& systems::World::Aircraft::Thruster::getMaxPower() {
        return this->maxPower;
    }

    float systems::World::Aircraft::getMass() {
        auto actor = this->entity.component<components::PhysicActor>();

        px::PxReal bodyMass = actor->getDynamicActor()->getMass();

        return bodyMass;
    }

    void systems::World::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
        const px::PxVec3 gravity = m_context.physic().getSceneGravity();

        for (auto &aircraft : aircrafts) {
            const float mass = aircraft.getMass();
            const float verticalPower = aircraft.thrusters.vertical.getPower();

            const float increaseRate = 0.01f;
            const float decreaseRate = 0.07f;

            auto actor = aircraft.entity.component<components::PhysicActor>()->getDynamicActor();

            px::PxTransform pose = actor->getGlobalPose();

            const float maxHeight = (aircraft.thrusters.vertical.getMaxPower() / mass) * glm::abs(gravity.y);
            float height = 0.0f;

            px::PxRaycastHit hit;
            hit.shape = nullptr;
            px::PxRaycastBuffer hit1;

            m_context.physic().getScene()->raycast(
                    pose.p - px::PxVec3(0.0f, 2.0f, 0.0f),
                    px::PxVec3(0.0f, -1.0f, 0.0f),
                    100.0f,
                    hit1,
                    px::PxHitFlag::eDEFAULT
            );
            hit = hit1.block;

            if(hit.shape)
            {
                height = hit.distance;
            }

            if (m_context.input().isPress(InputManager::KEY_W)) {
                aircraft.thrusters.back.increaseRate(increaseRate);
            } else {
                aircraft.thrusters.back.decreaseRate();
            }

            if (m_context.input().isPress(InputManager::KEY_S)) {
                aircraft.thrusters.front.increaseRate(increaseRate);
            } else {
                aircraft.thrusters.front.decreaseRate();
            }

            if (m_context.input().isPress(InputManager::KEY_A)) {
                aircraft.thrusters.left.increaseRate(increaseRate);
            } else {
                aircraft.thrusters.left.decreaseRate();
            }

            if (m_context.input().isPress(InputManager::KEY_D)) {
                aircraft.thrusters.right.increaseRate(increaseRate);
            } else {
                aircraft.thrusters.right.decreaseRate();
            }

            if (m_context.input().isPress(InputManager::KEY_SPACE)) {
                aircraft.thrusters.vertical.increaseRate(0.03f);
            } else {
                if (height > 5.0f) {
                    aircraft.thrusters.vertical.decreaseRate(0.005);
                }
            }

            if (height < 5.0f) {
                aircraft.thrusters.vertical.increaseRate(0.015f);
            }

            float heightFactor = glm::max(0.0f, 1.0f - (height / maxHeight));
            float verticalFactor = glm::max(-1.0f, aircraft.thrusters.vertical.getPower() / mass);

            const float x = ((-aircraft.thrusters.left.getPower()) + aircraft.thrusters.right.getPower()) / mass;
            const float y = verticalFactor * heightFactor;
            const float z = ((-aircraft.thrusters.back.getPower()) + aircraft.thrusters.front.getPower()) / mass;

//            console::info("vertical: %f, height: %f, y: %f", aircraft.thrusters.vertical.getPower(), height, y);

            px::PxVec3 dir{x, y, z};
            px::PxVec3 gravityDir = gravity / (1000.0f / static_cast<float>(dt));

            dir+= gravityDir;
            pose.p += dir;

            actor->setKinematicTarget(pose);
        }

        processQueue<Story>(loadedStory, [this, &es, &events](Story&){
//            auto turbineAsset = m_context.getAssets()->getModel("turbine");
//            auto leftWingAsset = m_context.getAssets()->getModel("left-wing");
//            auto rightWingAsset = m_context.getAssets()->getModel("right-wing");
//            auto engineAsset = m_context.getAssets()->getModel("engine");
//
            auto assetImage = m_context.getAssets()->getImage("metal");
            auto &image = assetImage->getImage();
            auto tex = std::make_shared<Texture>(Texture::Type::Diffuse, image);

            buildAircraft(es, events);
            buildHall(es, events);
        });
    }

    void systems::World::receive(const events::LoadStoryComplete &event) {
        loadedStory.push({});
    }

    void systems::World::buildAircraft(ex::EntityManager &es, ex::EventManager &events) {
        auto bodyMesh = m_context.meshes().create();
        auto wingMesh = m_context.meshes().create();

        {
            GeometryBuilder::BoxDescription bodyDesc;
            bodyDesc.width = 1.0f * 2.0f;
            bodyDesc.height = 1.0f * 2.0f;
            bodyDesc.depth = 3.0f * 2.0f;
            GeometryBuilder::Box(bodyMesh->geometry, bodyDesc);
            bodyMesh->material->setDiffuse(0.0f, 1.0f, 0.0f);

            GeometryBuilder::BoxDescription wingDesc;
            wingDesc.width = 3.0f * 2.0f;
            wingDesc.height = 1.0f * 2.0f;
            wingDesc.depth = 1.0f * 2.0f;
            GeometryBuilder::Box(wingMesh->geometry, wingDesc);
            wingMesh->material->setDiffuse(0.0f, 0.0f, 1.0f);
        }


        aircrafts.emplace_back();

        systems::World::Aircraft& aircraft = aircrafts.back();

        aircraft.entity = m_context.createObject(es);

        auto aircraftEntityActor = m_context.physic().createDynamicActor(vec3(0.0f, 20.0f, 0.0f));
        aircraftEntityActor->setRigidBodyFlag(px::PxRigidBodyFlag::eKINEMATIC, true);
        aircraftEntityActor->setMass(500.0f);

//        aircraftEntityActor->setAngularVelocity(px::PxVec3(0.0f));
//        aircraftEntityActor->setAngularDamping(0.0f);
//        px::PxRigidBodyExt::setMassAndUpdateInertia(*aircraftEntityActor, 50.0f);

        m_context.physic().createBoxGeometry(aircraftEntityActor, {1.0f, 1.0f, 3.0f});
        aircraft.entity.assign<components::PhysicActor>(aircraftEntityActor, aircraft.entity);
        aircraft.entity.assign<components::Transform>();
        aircraft.entity.assign<components::Mesh>(bodyMesh);
        aircraft.entity.assign<components::Renderable>();

        aircraft.shields.front.power = 100.0f;
        aircraft.energy.amount = 1000.0f;

        {
            aircraft.wings.left.entity = m_context.createObject(es);

            auto leftWingActor = m_context.physic().createDynamicActor(vec3(0.0f, 0.0f, 0.0f));
            aircraft.wings.left.entity.assign<components::PhysicActor>(leftWingActor, aircraft.wings.left.entity);
            m_context.physic().createBoxGeometry(leftWingActor, {3.0f, 1.0f, 1.0f});
            aircraft.wings.left.entity.assign<components::Transform>();
            aircraft.wings.left.entity.assign<components::Mesh>(wingMesh);
            aircraft.wings.left.entity.assign<components::Renderable>();

            px::PxFixedJoint* leftWingJoint = PxFixedJointCreate(
                    *m_context.physic().getPhysics(),
                    aircraftEntityActor, px::PxTransform(px::PxVec3(2.0f, 0.0f, 0.0f),	px::PxQuat(0, px::PxVec3(0,1,0))),
                    leftWingActor, px::PxTransform(px::PxVec3(8.0f, 0.0f, 0.0f),	px::PxQuat(0, px::PxVec3(0,1,0)))
            );

            aircraft.joints.push_back(leftWingJoint);
        }

        {
            aircraft.wings.right.entity = m_context.createObject(es);

            auto rightWingActor = m_context.physic().createDynamicActor(vec3(0.0f, 0.0f, 0.0f));
            aircraft.wings.right.entity.assign<components::PhysicActor>(rightWingActor, aircraft.wings.right.entity);
            m_context.physic().createBoxGeometry(rightWingActor, {3.0f, 1.0f, 1.0f});
            aircraft.wings.right.entity.assign<components::Transform>();
            aircraft.wings.right.entity.assign<components::Mesh>(wingMesh);
            aircraft.wings.right.entity.assign<components::Renderable>();

            px::PxFixedJoint* rightWingJoint = PxFixedJointCreate(
                    *m_context.physic().getPhysics(),
                    aircraftEntityActor, px::PxTransform(px::PxVec3(-2.0f, 5.0f, 0.0f),	px::PxQuat(0, px::PxVec3(0,1,0))),
                    rightWingActor, px::PxTransform(px::PxVec3(-8.0f, 5.0f, 0.0f),		px::PxQuat(0, px::PxVec3(0,1,0)))
            );

            aircraft.joints.push_back(rightWingJoint);
        }

        events.emit<events::ObjectCreate>(aircraft.entity);
        events.emit<events::ObjectCreate>(aircraft.wings.left.entity);
        events.emit<events::ObjectCreate>(aircraft.wings.right.entity);


    }

    void systems::World::buildHall(ex::EntityManager &es, ex::EventManager &events) {
        for (int i = -5; i < 5; i++) {
            {
                auto basePlaneMesh = m_context.meshes().create();

                GeometryBuilder::PlaneDescription desc;
                desc.height = 50.0f;
                desc.width = 80.0f;
                desc.heightSegments = 2;
                desc.widthSegments = 2;
                GeometryBuilder::Plane(basePlaneMesh->geometry, desc);

                basePlaneMesh->material->setColor(color::Rgb(0.15f));

                auto basePlaneEntity = m_context.createObject(es);

                auto basePlaneActor = m_context.physic().createStaticActor(vec3(0.0f, 0.0f, static_cast<float>(i) * desc.width));

                auto pose = basePlaneActor->getGlobalPose();
                pose.q.rotate(px::PxVec3(px::PxHalfPi, px::PxHalfPi, 0.0f));
                basePlaneActor->setGlobalPose(pose);

                m_context.physic().createBoxGeometry(basePlaneActor, px::PxVec3{desc.width * 0.5f, 0.1, desc.height * 0.5f});

                basePlaneEntity.assign<components::PhysicActor>(basePlaneActor, basePlaneEntity);
                basePlaneEntity.assign<components::Mesh>(basePlaneMesh);
                basePlaneEntity.assign<components::Renderable>();

                auto transform = basePlaneEntity.assign<components::Transform>();
                transform->position = vec3(0.0f, 0.0f, static_cast<float>(i) * desc.height);

                events.emit<events::ObjectCreate>(basePlaneEntity);
            }

            {
                auto leftPlaneMesh = m_context.meshes().create();

                GeometryBuilder::PlaneDescription desc;
                desc.height = 50.0f;
                desc.width = 20.0f;
                desc.heightSegments = 2;
                desc.widthSegments = 2;
                GeometryBuilder::Plane(leftPlaneMesh->geometry, desc);

                leftPlaneMesh->material->setColor(color::Rgb(0.2f));

                auto leftPlaneEntity = m_context.createObject(es);

                auto leftPlaneActor = m_context.physic().createStaticActor(vec3(-40.0f, 10.0f, static_cast<float>(i) * desc.height));
                m_context.physic().createBoxGeometry(leftPlaneActor, px::PxVec3{desc.width * 0.5f, 0.1, desc.height * 0.5f});

                leftPlaneEntity.assign<components::PhysicActor>(leftPlaneActor, leftPlaneEntity);
                leftPlaneEntity.assign<components::Mesh>(leftPlaneMesh);
                leftPlaneEntity.assign<components::Renderable>();

                auto originalPose = leftPlaneActor->getGlobalPose();
                auto newPose = px::PxTransform(originalPose.p, px::PxQuat(-px::PxHalfPi, px::PxVec3(0.0f, 0.0f, 1.0f)));

                leftPlaneActor->setGlobalPose(newPose);

                auto transform = leftPlaneEntity.assign<components::Transform>();
                transform->position = vec3(-40.0f, 10.0f, static_cast<float>(i) * desc.height);
                transform->rotate(vec3(0.0f, 0.0f, 1.0), -glm::half_pi<float>());

                events.emit<events::ObjectCreate>(leftPlaneEntity);
            }

            {
                auto rightPlaneMesh = m_context.meshes().create();

                GeometryBuilder::PlaneDescription desc;
                desc.height = 50.0f;
                desc.width = 20.0f;
                desc.heightSegments = 2;
                desc.widthSegments = 2;
                GeometryBuilder::Plane(rightPlaneMesh->geometry, desc);

                rightPlaneMesh->material->setColor(color::Rgb(0.2f));

                auto rightPlaneEntity = m_context.createObject(es);

                auto rightPlaneActor = m_context.physic().createStaticActor(vec3(40.0f, 10.0f, static_cast<float>(i) * desc.height));
                m_context.physic().createBoxGeometry(rightPlaneActor, px::PxVec3{desc.width * 0.5f, 0.1, desc.height * 0.5f});

                rightPlaneEntity.assign<components::PhysicActor>(rightPlaneActor, rightPlaneEntity);
                rightPlaneEntity.assign<components::Mesh>(rightPlaneMesh);
                rightPlaneEntity.assign<components::Renderable>();

                auto originalPose = rightPlaneActor->getGlobalPose();
                auto newPose = px::PxTransform(originalPose.p, px::PxQuat(-px::PxHalfPi, px::PxVec3(0.0f, 0.0f, 1.0f)));

                rightPlaneActor->setGlobalPose(newPose);

                auto transform = rightPlaneEntity.assign<components::Transform>();
                transform->position = vec3(40.0f, 10.0f, static_cast<float>(i) * desc.height);
                transform->rotate(vec3(0.0f, 0.0f, 1.0), glm::half_pi<float>());

                events.emit<events::ObjectCreate>(rightPlaneEntity);
            }
        }
    }

    systems::World::Aircraft::Aircraft(systems::World::Aircraft &&other) noexcept
        : joints(std::move(other.joints))
        , wings(wings)
        , thrusters(thrusters)
        , shields(shields)
        , energy(energy)
    {

    }

    systems::World::Aircraft::~Aircraft() {
        console::info("aircraft destroyed");

        for (auto &joint : joints) {
            joint->release();
        }
        joints.clear();
    }
}