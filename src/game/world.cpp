#include <algorithm>
#include <thread>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assert.h>
#include "world.h"
#include "systems/physic.h"
#include "systems/camera.h"
#include "systems/render.h"
#include "systems/ball_shot.h"
#include "systems/animations.h"
#include "systems/character_control.h"
#include "systems/input.h"
#include "systems/char_control.h"
#include "systems/light_helpers.h"
#include "systems/day_time.h"
#include "systems/weapons.h"
#include "systems/sky.h"
#include "components/skin.h"
#include "components/equipment.h"
#include "components/inventory.h"
#include "components/transform.h"
#include "components/state.h"
#include "components/renderable.h"
#include "events/camera_look_at.h"
#include "events/character_create.h"
#include "events/character_remove.h"
#include "events/input.h"
#include "events/light_add.h"
#include "events/light_remove.h"
#include "events/render_setup_model.h"
#include "components/light_point.h"
#include "components/light_directional.h"
#include "components/inventory.h"
#include "components/meshes.h"
#include "desc/light_spot.h"
#include "strategies/weapons/default.h"
#include "strategies/weapons/rocket_launcher.h"
#include "world_importer.h"
#include "../math/Box3.h"
#include "../core/material_builder.h"
#include "components/light_spot.h"

namespace game {
    World::World() : name("") {

    }

    void World::destroyRenderer() {
        renderer->destroy();
    }

    void World::setup() {
        systems.add<systems::Render>(this);
        systems.add<systems::Input>(this);
        systems.add<systems::Camera>(this);
        systems.add<systems::CharControl>(this);
        systems.add<systems::CharacterControl>(this); // deprecated
        systems.add<systems::Animations>(this);
        systems.add<systems::Physic>(this);
        systems.add<systems::BallShot>(this);
        systems.add<systems::LightHelpers>(this);
        systems.add<systems::DayTime>(this);
        systems.add<systems::Sky>(this);
        systems.add<systems::Weapons>(this);

        systems.configure();

        camera = systems.system<game::systems::Camera>();
        physic = systems.system<game::systems::Physic>();
        weapons = systems.system<game::systems::Weapons>();
        renderer = systems.system<game::systems::Render>();

        registerWeapon(new strategy::WeaponsDefault());
        registerWeapon(new strategy::WeaponsRocketLauncher());

        physic->postConfigure(events);

        console::info("world configure success");
    }

    void World::update(ex::TimeDelta dt) {
        profiling::Mark m = profiling::mark(systemProfiler, "WorldUpdate");

        // pre update
        PROFILE(systemProfiler, "Input", systems.update<game::systems::Input>(dt));
        PROFILE(systemProfiler, "Camera", systems.update<game::systems::Camera>(dt));
        PROFILE(systemProfiler, "CharControl", systems.update<game::systems::CharControl>(dt));
        PROFILE(systemProfiler, "Physic", systems.update<game::systems::Physic>(dt));
        PROFILE(systemProfiler, "DayTime", systems.update<game::systems::DayTime>(dt));
        PROFILE(systemProfiler, "DayTime", systems.update<game::systems::Sky>(dt));

        // main update
        PROFILE(systemProfiler, "Weapons", systems.update<game::systems::Weapons>(dt));
        PROFILE(systemProfiler, "BallShot", systems.update<game::systems::BallShot>(dt));

        for (auto &script : scripts) {
            PROFILE(systemProfiler, script->getName(), script->evalUpdate(static_cast<float>(dt)));
        }

        // post update
        PROFILE(systemProfiler, "Animations", systems.update<game::systems::Animations>(dt));
        PROFILE(systemProfiler, "LightHelpers", systems.update<game::systems::LightHelpers>(dt));
    }

    void World::flush() {
        WorldTask *task = nullptr;

        while (!completedTasks.empty()) {
            bool status = completedTasks.pop(task);
            if (!status) {
                continue;
            }

            task->onFlush();
        }
    }

    void World::render(ex::TimeDelta dt) {
        PROFILE(systemProfiler, "Render", systems.update<systems::Render>(dt));
    }

    bool World::registerWeapon(strategy::WeaponsBase *system) {
        return weapons->registerStrategy(system);
    }

    World::Weapon World::createWeapon() {
        ex::Entity entity = entities.create();

        desc::Weapon description;
        description.name = "rocket launcher";
        description.fireRate = 0.1f;

        strategy::WeaponsBase *strategy = weapons->getWeaponStrategy("RocketLauncher");

        World::Weapon weapon(entity, description, strategy);

        return weapon;
    }

    void World::destroy() {
        unloadScripts();

        for (auto &it : objects) {
            delete it.second;
        }

        objects.erase(objects.begin(), objects.end());

        entities.reset();

        console::info("world %s destroyed", name);
    }

    void World::setRenderSize(renderer::Dimension width, renderer::Dimension height) {
        events.emit<events::RenderResize>(width, height);
    }

    void World::setInput(int place, InputHandler *ih) {
        console::info(" SET INPUT %i", ih);
        context.input = ih;
        events.emit<events::InputSetHandler>(ih, place);
    }

    void World::removeInput(systems::Input::InputPlace place) {

    }

    game::Context &World::getContext() {
        return context;
    }

    game::WorldObject *World::getUserControlCharacter() {
        game::WorldObject *object = nullptr;

        ex::ComponentHandle<components::UserControl> userControl;
        for (ex::Entity entity : entities.entities_with_components(userControl)) {
            auto it = objects.find(entity.id());
            if (it != objects.end()) {
                object = it->second;
            }
        }

        return object;
    }

    /**
     * --------------------------------------------------
     * Camera methods
     * --------------------------------------------------
     */

    void World::setCameraSettings(float fov, float aspect, float near, float far) {
        camera->setSettings(fov, aspect, near, far);
    }

    void World::cameraLookAt(vec3 target) {
        camera->lookAt(target);
    }


    /**
     * --------------------------------------------------
     * Light methods
     * --------------------------------------------------
     */

    void World::addLight(desc::LightPointDesc lightDescription) {
        entityx::Entity entity = entities.create();

        components::LightPoint light;
        light.setAmbient(lightDescription.ambient);
        light.setDiffuse(lightDescription.diffuse);
        light.setSpecular(lightDescription.specular);
        light.setPosition(lightDescription.position);
        light.setAttenuation(lightDescription.constant, lightDescription.linear, lightDescription.quadratic);

        entity.assign<components::LightPoint>(light);
        entity.assign<components::Transform>(lightDescription.position);

        events.emit<events::LightAdd>(entity);
    }

    void World::addLight(desc::LightDirectionalDesc lightDescription) {
        entityx::Entity entity = entities.create();

        components::LightDirectional light;
        light.setAmbient(lightDescription.ambient);
        light.setDiffuse(lightDescription.diffuse);
        light.setSpecular(lightDescription.specular);
        light.setDirection(lightDescription.direction);

        entity.assign<components::LightDirectional>(light);
        entity.assign<components::Transform>(lightDescription.position);

        events.emit<events::LightAdd>(entity);
    }

    void World::addLight(desc::LightSpotDesc lightDescription) {
        entityx::Entity entity = entities.create();


        game::components::LightSpot light;
        light.setAmbient(lightDescription.ambient);
        light.setDiffuse(lightDescription.diffuse);
        light.setSpecular(lightDescription.specular);
        light.setPosition(lightDescription.position);
        light.setDirection(lightDescription.direction);
        light.setAttenuation(lightDescription.constant, lightDescription.linear, lightDescription.quadratic);
        light.setCutoff(lightDescription.cutOff, lightDescription.outerCutOff);

        entity.assign<game::components::LightSpot>(light);
        entity.assign<game::components::Transform>(lightDescription.position);

        events.emit<events::LightAdd>(entity);
    }

    void World::setLightDebug(bool value) {
        events.emit<events::LightHelperShow>(value);
    }


    /**
     * --------------------------------------------------
     * Physic methods
     * --------------------------------------------------
     */

    void World::forcePush(ex::Entity entity, vec3 direction, float force) {
        events.emit<events::PhysicForce>(entity, direction, force);
    }

    void World::impactWave(vec3 position, vec3 direction) {
        physic->wave(position, direction);
    }

    void World::spawn(game::WorldObject *object) {
        vec3 position = object->transform->getPosition();
        spawn(object, position);
    }

    void World::spawn(game::WorldObject *object, const vec3 &position) {
        object->transform->setPosition(position);
        physic->spawn(object);
    }

    void World::setPhysicsDebug(bool value) {
        physic->setDrawDebug(value);
    }

    void World::setGravity(vec3 dir) {
        physic->setSceneGravity(dir);
    }

    void World::importAssets(std::shared_ptr<Assets> &newAssets) {
        // @todo: cleanup world items
        assets.reset();
        assets = newAssets;
        context.assets = assets;
    }

    void World::reloadAssetsScripts() {
//        unloadScripts();

        for (auto &it : assets->getScripts()) {
            assets::Script *assetScript = &it.second;
            auto script = new game::Script(assetScript);

            Script::INIT_STATUS status = script->init(this);

            if (status == Script::INIT_STATUS::SUCCESS) {
                scripts.push_back(script);
            }
        }

        for (auto &it : assets->getModels()) {
            auto &assetModel = it.second;
        }
    }

    void World::reloadRenderAssets() {
//        unloadTextures();

        for (auto &it : assets->getTextures()) {
            renderer->addTexture(&it.second);
        }

        for (auto &it : assets->getShaders()) {
            renderer->addShader(&it.second);
        }

        for (auto &it : assets->getMaterials()) {
            renderer->addMaterial(&it.second);
        }
    }

    void World::load() {
        reloadAssetsScripts();
        reloadRenderAssets();
    }

    void World::unload() {
        unloadScripts();
    };

    void World::unloadScripts() {
        for (auto &script : scripts) {
            delete script;
        }

        scripts.erase(scripts.begin(), scripts.end());
    }

    void World::unloadTextures() {
        for (auto &it : assets->getTextures()) {
            renderer->removeTexture(&it.second);
        }
    }

    const profiling::ProfileTimings &World::getSystemProfiler() const {
        return systemProfiler;
    }

    std::shared_ptr<Material> World::findMaterial(const std::string &name) {
        assets::Material *assetMaterial = assets->getMaterial(name);

        if (assetMaterial == nullptr) {
            return nullptr;
        }

        return assetMaterial->getMaterial();
    }

    void World::generateBaseTerrain() {
//        std::shared_ptr<Material> material = assetMaterial->getMaterial();
//        assets::Material *assetMaterial = assets->createMaterial("waterball");

//        auto assetMaterial = assets->createMaterial("ground");
//        auto material = std::make_shared<::Material>();

//        std::shared_ptr<::Material> material;
//        material.reset(new ::Material(*assetMaterial->getMaterial()));
//
//        auto mesh = Mesh::Create(material);
//
//        GeometryBuilder::PlaneDescription planeDescription;
//        planeDescription.width = 50.0f;
//        planeDescription.height = 50.0f;
//        planeDescription.widthSegments = 16;
//        planeDescription.heightSegments = 16;
//        GeometryBuilder::Plane(mesh->geometry, planeDescription);
//
//        game::WorldObject *object = createStaticObject();
//
//        object->rotate(vec3(0.0f, 1.0f, 0.0f), glm::radians(90.0f));
//        object->rotate(vec3(1.0f, 0.0f, 0.0f), glm::radians(90.0f));
//
//        setObjectMesh(object, mesh);
//        spawn(object, vec3(0.0f));
//        setupRenderMesh(object->getEntity());


        assets::Texture *texture = assets->getTexture("terrain");

        if (texture != nullptr) {
            ex::Entity baseTerrain = createTerrain(texture->getImage());
            showObject(baseTerrain);
            events.emit<events::RenderSetupModel>(baseTerrain);
        }
    }


    /* ----- API v2 ----- */

    game::WorldObject *World::createObject() {
        createObject(vec3(0.0f));
    }

    game::WorldObject *World::createObject(const glm::vec3 &pos) {
        ex::Entity entity = entities.create();
        auto object = new game::WorldObject(entity);
        object->setPosition(pos);

        objects.insert({object->getId(), object});

        return object;
    }

    game::WorldObject *World::createStaticObject() {
        createStaticObject(vec3(0.0f));
    }

    game::WorldObject *World::createDynamicObject() {
        createDynamicObject(vec3(0.0f));
    }

    game::WorldObject *World::createStaticObject(const glm::vec3 &pos) {
        auto *object = createObject(pos);
        physic->makeStatic(object);

        return object;
    }

    game::WorldObject *World::createStaticObject(const game::WorldObject::Description& description) {
        auto *object = createObject(description.position);

        physic->makeStatic(object);

        return object;
    }

    game::WorldObject *World::createDynamicObject(const glm::vec3 &pos) {
        game::WorldObject::Description description(pos);

        auto *object = createObject(pos);
        physic->makeDynamic(object, description);

        return object;
    }

    game::WorldObject *World::createDynamicObject(const game::WorldObject::Description& description) {
        auto *object = createObject(description.position);

        physic->makeDynamic(object, description);

        return object;
    }

    game::WorldObject *World::createCharacterObject() {
        auto *object = createObject();

        ex::Entity entity = object->getEntity();

//        auto model = entity.assign<c::Meshes>();
//        auto skin = entity.assign<c::Skin>();
        entity.assign<c::Character>();
        entity.assign<c::Inventory>();
        entity.assign<c::Equipment>();
//
////        ModelBuilder::FromAi(model.get(), resource, assets);
////        SkinBuilder::FromAi(skin.get(), resource);

//        auto nodeIndexes = skin->getNodeIndexes();
//        model->reindexMeshBones(nodeIndexes);

        events.emit<events::CharacterCreate>();

        return object;
    }

    game::WorldObject *World::createLightObject(const desc::LightDirectionalDesc &description) {
        ex::Entity entity = entities.create();
        auto object = new game::WorldObject(entity);

        components::LightDirectional light;
        light.setAmbient(description.ambient);
        light.setDiffuse(description.diffuse);
        light.setSpecular(description.specular);
        light.setDirection(description.direction);

        object->addComponent<components::LightDirectional>(light);
        object->setPosition(description.position);

        objects.insert({object->getId(), object});
        events.emit<events::LightAdd>(entity);

        return object;
    };

    game::WorldObject *World::createLightObject(const game::desc::LightPointDesc &description) {
        ex::Entity entity = entities.create();
        auto object = new game::WorldObject(entity);

        components::LightPoint light;
        light.setAmbient(description.ambient);
        light.setDiffuse(description.diffuse);
        light.setSpecular(description.specular);
        light.setPosition(description.position);
        light.setAttenuation(description.constant, description.linear, description.quadratic);

        object->addComponent<components::LightPoint>(light);
        object->setPosition(description.position);

        objects.insert({object->getId(), object});
        events.emit<events::LightAdd>(entity);

        return object;
    };

    game::WorldObject *World::createLightObject(const game::desc::LightSpotDesc &description) {
        ex::Entity entity = entities.create();
        auto object = new game::WorldObject(entity);

        game::components::LightSpot light;
        light.setAmbient(description.ambient);
        light.setDiffuse(description.diffuse);
        light.setSpecular(description.specular);
        light.setPosition(description.position);
        light.setDirection(description.direction);
        light.setAttenuation(description.constant, description.linear, description.quadratic);
        light.setCutoff(description.cutOff, description.outerCutOff);

        object->addComponent<components::LightSpot>(light);
        object->setPosition(description.position);

        objects.insert({object->getId(), object});
        events.emit<events::LightAdd>(entity);

        return object;
    };

    void World::destroyObject(game::WorldObject *object) {
        if (object != nullptr) {
            game::WorldObject::Id objectId = object->getId();

            auto it = objects.find(objectId);
            if (it != objects.end()) {
                objects.erase(it);
            }

            delete object;
            object = nullptr;
        }
    }

    ex::Entity World::createTerrain(const std::shared_ptr<ImageData> &image) {
        ex::Entity terrain = entities.create();

        game::HeightMap *heightmap = physic->createHeightMap(image);
        physx::PxRigidStatic *terrainActor = physic->generateTerrain(*heightmap, 200.0f, 200.0f);

        std::shared_ptr<Mesh> mesh = physic->generateTerrainMesh(terrainActor, *heightmap);
        mesh->material = findMaterial("ground");

        terrain.assign<c::Meshes>(mesh);
        terrain.assign<c::Transform>(terrainActor->getGlobalPose());

        delete heightmap;

        return terrain;
    }

    void World::hideObject(ex::Entity &entity) {
        entity.remove<c::Renderable>();
    }

    void World::showObject(ex::Entity &entity) {
        entity.assign<c::Renderable>();
    }

    assets::Model *World::findAssetModel(const std::string &name) {
        assets::Model *asset = assets->getModel(name);

        if (!asset->isLoaded()) {
            asyncLoad(asset);
        }

        return asset;
    }

    void World::setObjectModel(game::WorldObject *object, assets::Model *asset) {
        if (asset != nullptr) {
            World *world = this;

            if (asset->isLoaded()) {
                applyObjectAsset(object, asset);
            }

            asset->onLoad.connect([object, asset, world]() {
                world->applyObjectAsset(object, asset);
            });
        }
    }

    void World::hideObject(game::WorldObject *object) {
        ex::Entity entity = object->getEntity();
        hideObject(entity);
    }

    void World::showObject(game::WorldObject *object) {
        ex::Entity entity = object->getEntity();
        showObject(entity);
    }

    void World::setObjectMesh(game::WorldObject *object, std::shared_ptr<Mesh> &mesh) {
        ex::Entity entity = object->getEntity();
        entity.assign<c::Meshes>(mesh);
    }

    void World::makeControlled(game::WorldObject *object) {
        events.emit<events::MakeControlled>(object->getEntity());
    }

    void World::makeUnControlled(game::WorldObject *object) {
        events.emit<events::MakeUnControlled>(object->getEntity());
    }

    void World::forcePush(game::WorldObject *object, const vec3 &direction, const float &force) {
        auto entity = object->getEntity();
        events.emit<events::PhysicForce>(entity, direction, force);
    }

    void World::asyncLoad(assets::Model *asset) {
        asyncLoadModel(asset);
    }

    void World::asyncLoadModel(assets::Model *asset) {
        auto *context = new WorldTaskContext(this);
        context->setData(asset);

        WorldTask *task = new WorldModelLoadTask(context);

        tasks.push(task);
    }

    bool World::hasTasks() {
        return !tasks.empty();
    }

    WorldTask *World::popTaskToPerform() {
        WorldTask *task = nullptr;

        bool status = tasks.pop(task);

        if (!status || task == NULL) {
            task = nullptr;
        }

        return task;
    }

    void World::performTask(WorldTask *task) {
        assert(task != nullptr);

        task->onStart();
        task->onFinish();

        completedTasks.push(task);
    }

    std::shared_ptr<Assets> World::getAssets() {
        return assets;
    }

    void World::setupRenderMesh(const ex::Entity &entity) {
        events.emit<events::RenderSetupModel>(entity);
    }

    void World::applyObjectAsset(game::WorldObject *object, assets::Model *asset) {
        ex::Entity entity = object->getEntity();

        if (entity.has_component<c::Meshes>()) {
            entity.remove<c::Meshes>();
        }

        entity.assign<c::Meshes>(asset->getMeshes());

        physic->computeBoundingBox(object);

//        if (scene->HasAnimations()) {
//            aiScene const *scene = asset->getScene();
//            auto skin = entity.assign<c::Skin>();
//            SkinBuilder::FromAi(skin.get(), scene);
//
//            // @todo: реиндексацию можно делать один раз, но основе только скелета
//            auto nodeIndexes = skin->getNodeIndexes();
//            meshes->reindexBones(nodeIndexes);
//
//            const std::string animationName = asset->getOption("animation");
//            if (!animationName.empty()) {
//                skin->setCurrentAnimation(animationName);
//            }
//        }

        setupRenderMesh(entity);
    }

    void World::setRenderer(renderer::Renderer *renderer) {
        this->renderer->setRenderer(renderer);
    }
}
