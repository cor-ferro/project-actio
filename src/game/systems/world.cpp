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

    void systems::World::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
        for (auto &aircraft : aircrafts) {
            pos_i+= 0.01f;
            auto actor = aircraft.entity.component<components::PhysicActor>()->getDynamicActor();
            px::PxTransform target{0.0f, 0.0f, 0.0f};
            if (actor->getKinematicTarget(target)) {
                console::info("move");
                target.p.z+= 0.1f;
                actor->setKinematicTarget(target);
            } else {
                actor->setKinematicTarget({0.0f, 3.0f, pos_i});
            }


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

        auto aircraftEntityActor = m_context.physic().createDynamicActor(vec3(0.0f, 3.0f, 0.0f));
        aircraftEntityActor->setRigidBodyFlag(px::PxRigidBodyFlag::eKINEMATIC, true);
        m_context.physic().createBoxGeometry(aircraftEntityActor, {1.0f, 1.0f, 3.0f});
        aircraft.entity.assign<components::PhysicActor>(aircraftEntityActor, aircraft.entity);
        aircraft.entity.assign<components::Transform>();
        aircraft.entity.assign<components::Mesh>(bodyMesh);
        aircraft.entity.assign<components::Renderable>();

        aircraft.shields.front.power = 100.0f;
        aircraft.energy.amount = 1000.0f;

        px::PxQuat y90Rote(px::PxHalfPi, px::PxVec3(0.0f,1.0f,0.0f));


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