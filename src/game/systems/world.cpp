#include "world.h"
#include "../../core/geometry_builder.h"
#include "../components/mesh.h"
#include "../components/renderable.h"
#include "../components/physic_actor.h"
#include "../components/camera.h"
#include "../components/target.h"
#include "../components/light_directional.h"
#include "../events/object_create.h"

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
        processQueue<Story>(loadedStory, [this, &es, &events](Story&){
            auto assetImage = m_context.getAssets()->getImage("metal");
            auto &image = assetImage->getImage();
            auto tex = std::make_shared<Texture>(Texture::Type::Diffuse, image);

            // plane
            {
                auto planeMesh = m_context.meshes().create();
//                planeMesh->material->setDiffuse(0.0f, 1.0f, 0.0f);
                planeMesh->material->setDiffuseMap(tex);

                GeometryBuilder::PlaneDescription planeDesc;
                planeDesc.width = 20.0f;
                planeDesc.height = 20.0f;
                planeDesc.widthSegments = 16;
                planeDesc.heightSegments = 16;
                GeometryBuilder::Plane(planeMesh->geometry, planeDesc);

                auto plane = m_context.createObject(es);

                auto physicActor = m_context.physic().createStaticActor(vec3(0.0f));
                m_context.physic().createBoxGeometry(physicActor, {5.0f, 5.0f, 1.0f});

                plane.assign<components::Mesh>(planeMesh);
                plane.assign<components::PhysicActor>(physicActor);
                plane.assign<components::Transform>();
                plane.assign<components::Renderable>();

                plane.component<components::Transform>()->rotateY(glm::half_pi<float>());
                plane.component<components::Transform>()->rotateX(glm::half_pi<float>());

                events.emit<events::ObjectCreate>(plane);
            }


            // box
            {
                auto boxMesh = m_context.meshes().create();

//                boxMesh->material->setDiffuse(0.0f, 0.0f, 1.0f);
                boxMesh->material->setDiffuseMap(tex);

                GeometryBuilder::BoxDescription boxDesc;
                boxDesc.width = 3.0f;
                boxDesc.height = 3.0f;
                boxDesc.heightSegments = 1;
                boxDesc.widthSegments = 1;
                GeometryBuilder::Box(boxMesh->geometry, boxDesc);

                auto box = m_context.createObject(es);

                auto boxPhysicActor = m_context.physic().createStaticActor(vec3(0.0f));
                m_context.physic().createBoxGeometry(boxPhysicActor, {2.0f, 2.0f, 2.0f});
                box.assign<components::Mesh>(boxMesh);
                box.assign<components::PhysicActor>(boxPhysicActor);
                box.assign<components::Transform>();
                box.assign<components::Renderable>();

                events.emit<events::ObjectCreate>(box);
            }
        });
    }

    void systems::World::receive(const events::LoadStoryComplete &event) {
        loadedStory.push({});
    }
}