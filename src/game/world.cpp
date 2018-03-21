#include "world.h"
#include "systems/movement.h"
#include "systems/render.h"
#include "components/position.h"
#include "components/state.h"
#include "components/renderable.h"

namespace game {
    World::World()
        : name("")
        , time(0)
    {
        systems.add<systems::Movement>();
        systems.add<systems::Render>();
        systems.configure();

        initPhysics();
    }

    void World::update(TimeDelta dt)
    {
        systems.update<systems::Movement>(dt);
    }

    void World::render(TimeDelta dt)
    {
        systems.update<systems::Render>(dt);
    }

    inline void World::spawn(Character* character)
    {
        spawn(character, vec3(0.0f, 0.0f, 0.0f));
    }

    void World::spawn(Character* character, vec3 position)
    {
        entityx::Entity entity = entities.create();
        entity.assign<components::State>();
        entity.assign<components::Renderable>();
        entity.assign<components::Position>(0.0f, 0.0f, 0.0f);

        // WorldObject<Character> object;
        // object.object = character;
        // object.position = position;
        // object.frozen = false;
        // object.hidden = false;

        // characters.push_back(object);
    }

    void World::initPhysics()
    {
        gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gAllocator, gErrorCallback);
        gPvd = PxCreatePvd(*gFoundation);
        gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

        gDispatcher = PxDefaultCpuDispatcherCreate(4);

        PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
        sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);
        sceneDesc.cpuDispatcher = gDispatcher;
        sceneDesc.filterShader = PxDefaultSimulationFilterShader;
        sceneDesc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;
        sceneDesc.flags |= PxSceneFlag::eENABLE_PCM;
        sceneDesc.flags |= PxSceneFlag::eENABLE_STABILIZATION;
        sceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;
        sceneDesc.gpuMaxNumPartitions = 8;

        gScene = gPhysics->createScene(sceneDesc);

        pxMaterials.insert({ "default", gPhysics->createMaterial(0.5f, 0.5f, 0.6f) });

        console::info("world physics inited");
    }

    void World::add()
    {
        // PxMaterial* gMaterial = pxMaterials.find("default")->second;
        
        // PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *gMaterial);
        // gScene->addActor(*groundPlane);

        // PxRigidDynamic* ball = createDynamic(PxTransform(PxVec3(0, 20, 0)), PxSphereGeometry(3), PxVec3(0, -25, -5));
	    // PxRigidBodyExt::updateMassAndInertia(*ball, 1000.0f);
        // gScene->addActor(*ball);
    }

    void World::remove()
    {

    }

    PxRigidDynamic* World::createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity)
    {
        PxMaterial* gMaterial = pxMaterials.find("default")->second;

        PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, t, geometry, *gMaterial, 10.0f);
        dynamic->setAngularDamping(0.5f);
        dynamic->setLinearVelocity(velocity);

        return dynamic;
    }

    void World::stepPhysics()
    {
        PX_UNUSED(false);

        gScene->simulate(1.0f/60.0f);
        gScene->fetchResults(true);
    }

    void World::applyPhysics()
    {
        // if (!ball->isSleeping()) {
        //     PxTransform ballTransform = ball->getGlobalPose();

        //     sphereModel->setPosition(ballTransform.p.x, ballTransform.p.y, ballTransform.p.z);
        //     sphereModel->setQuaternion(ballTransform.q.x, ballTransform.q.y, ballTransform.q.z, ballTransform.q.w);
        // }
    }

    void World::cleanupPhysics()
    {
        if (gScene) {
            gScene->release();
            gScene = NULL;
        }

        if (gDispatcher) {
            gDispatcher->release();
            gDispatcher = NULL;
        }

        if (gPhysics) {
            gPhysics->release();
            gPhysics = NULL;
        }

        if (gFoundation) {
            gFoundation->release();
            gFoundation = NULL;
        }

        console::info("physics released");
    }

    void World::initFromFile(const Resource::File& file) {
        IniLoader iniLoader;
        iniLoader.defineSection("world", {"models", "lights", "skybox", "camera"});
        iniLoader.defineSection("model", {"File", "FlipUv", "Animation", "Position", "Rotation", "RotationAngle", "Scale"});
        iniLoader.defineSection("camera", {"Type", "Position", "Target", "AspectRatio", "Fov", "Near", "Far"});
        iniLoader.defineSection("light", {"Type", "Ambient", "Diffuse", "Specular", "Position", "Constant", "Linear", "Quadratic", "Direction"});
        iniLoader.defineSection("skybox", {"PositiveX", "NegativeX", "PositiveY", "NegativeY", "PositiveZ", "NegativeZ"});
        iniLoader.load(file.getPath());

        const std::vector<IniLoader::Section>* models = iniLoader.getSections("model");
        if (models != nullptr) {
            for (auto it = models->begin(); it != models->end(); ++it) {
                const IniLoader::Section& section = (*it);

                console::info("create entity");
            }

            // for (const IniLoader::Section& section : models) {
                // vec3 pos = section.getFieldVec("Position");
                
                // console::info("create entity");
                // entityx::Entity entity = entities.create();
                // entity.assign<components::Position>(pos.x, pos.y, pos.z);
            // }
        }
    }

    void World::destroy()
    {
        for (WorldObject<Character>& object : characters)
        {
            if (object.object != nullptr) {
                delete object.object;
                object.object = nullptr;
            }
        }

        characters.erase(characters.begin(), characters.end());

        cleanupPhysics();

        for (auto& x : pxMaterials)
        {
            // delete x->second;
            // x->second = nullptr;
        }

        pxMaterials.erase(pxMaterials.begin(), pxMaterials.end());

        console::infop("world %s destroyed", name.c_str());
    }
}