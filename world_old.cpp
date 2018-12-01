#include <algorithm>
#include <thread>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assert.h>
#include "world_old.h"

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
#include "events/render_update_mesh.h"
#include "components/light_point.h"
#include "components/light_directional.h"
#include "components/inventory.h"
#include "components/mesh.h"
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

    void World::setup() {
        context.setup();

        registerWeapon(new strategy::WeaponsDefault());
        registerWeapon(new strategy::WeaponsRocketLauncher());

        console::info("world configure success");
    }

    void World::update(const ex::TimeDelta& dt) {
        context.update(dt);
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

    void World::render(const ex::TimeDelta& dt) {
        context.render(dt);
    }

    bool World::registerWeapon(strategy::WeaponStrategy *system) {
        return context.weapons()->registerStrategy(system);
    }

    void World::destroy() {
        unloadScripts();

        for (auto &it : objects) {
            delete it.second;
        }

        objects.erase(objects.begin(), objects.end());

        context.reset();

        console::info("world %s destroyed", name);
    }

    void World::setRenderSize(renderer::Dimension width, renderer::Dimension height) {
        context.renderer()->setSize(width, height);
    }

    void World::setInput(int place, InputManager *ih) {
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
        context.camera()->setSettings(fov, aspect, near, far);
    }

    void World::cameraLookAt(const vec3& target) {
        context.camera()->lookAt(target);
    }


    /**
     * --------------------------------------------------
     * Light methods
     * --------------------------------------------------
     */

    void World::addLight(desc::LightPointDesc lightDescription) {
        context.createLight(lightDescription);
    }

    void World::addLight(desc::LightDirectionalDesc lightDescription) {
        context.createLight(lightDescription);
    }

    void World::addLight(desc::LightSpotDesc lightDescription) {
        context.createLight(lightDescription);
    }

    void World::setLightDebug(bool value) {
        context.lightHelpers()->show(value);
    }

    void World::spawn(game::WorldObject *object) {
        vec3 position = object->transform->getPosition();
        spawn(object, position);
    }

    void World::spawn(game::WorldObject *object, const vec3 &position) {
        object->transform->setPosition(position);
        context.physic()->spawn(object);
    }

    void World::setPhysicsDebug(bool value) {
        context.physic()->setDrawDebug(value);
    }

    void World::setGravity(const vec3& dir) {
        context.physic()->setSceneGravity(dir);
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
            context.renderer()->addTexture(&it.second);
        }

        for (auto &it : assets->getShaders()) {
            context.renderer()->addShader(&it.second);
        }

        for (auto &it : assets->getMaterials()) {
            context.renderer()->addMaterial(&it.second);
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
            context.renderer()->removeTexture(&it.second);
        }
    }

    const profiling::ProfileTimings &World::getSystemProfiler() const {
        return context.getSystemProfiler();
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
            events.emit<events::RenderUpdateMesh>(baseTerrain);
        }
    }


    /* ----- API v2 ----- */

    game::WorldObject *World::createObject() {
        createObject(vec3(0.0f));
    }

    game::WorldObject *World::createObject(const glm::vec3 &pos) {
        ex::Entity entity = context.createEntity();

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
        context.physic()->makeStatic(object);

        return object;
    }

    game::WorldObject *World::createStaticObject(const game::WorldObject::Description& description) {
        auto *object = createObject(description.position);
        context.physic()->makeStatic(object);

        return object;
    }

    game::WorldObject *World::createDynamicObject(const glm::vec3 &pos) {
        game::WorldObject::Description description(pos);

        auto *object = createObject(pos);
        context.physic()->makeDynamic(object, description);

        return object;
    }

    game::WorldObject *World::createDynamicObject(const game::WorldObject::Description& description) {
        auto *object = createObject(description.position);

        context.physic()->makeDynamic(object, description);

        return object;
    }

    game::WorldObject *World::createCharacterObject() {
        auto *object = createObject();

        ex::Entity entity = object->getEntity();

//        auto model = entity.assign<c::Mesh>();
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
        ex::Entity entity = context.createEntity();
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
        ex::Entity entity = context.createEntity();
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
        ex::Entity entity = context.createEntity();
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
        ex::Entity terrain = context.createEntity();

        game::HeightMap *heightmap = context.physic()->createHeightMap(image);
        physx::PxRigidStatic *terrainActor = context.physic()->generateTerrain(*heightmap, 200.0f, 200.0f);

        std::shared_ptr<Mesh> mesh = context.physic()->generateTerrainMesh(terrainActor, *heightmap);
        mesh->material = findMaterial("ground");

        terrain.assign<c::Mesh>(mesh);
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
        entity.assign<c::Mesh>(mesh);
    }

    void World::makeControlled(game::WorldObject *object) {
        events.emit<events::MakeControlled>(object->getEntity());-
    }

    void World::makeUnControlled(game::WorldObject *object) {
        events.emit<events::MakeUnControlled>(object->getEntity());
    }

    void World::createInventory(game::WorldObject *object) {
        if (!object->hasComponent<c::Character>()) {
            console::warn("try create inventory on non-character object");
        }

        if (!object->hasComponent<c::Equipment>()) {
            object->addComponent<c::Equipment>();
        }

        if (!object->hasComponent<c::Inventory>()) {
            object->addComponent<c::Inventory>();
        }
    }

    void World::assignDefaultWeapon(game::WorldObject *object) {
        if (object->hasComponent<c::Equipment>()) {
            ex::Entity weapon = context.createEntity();

            auto strategy = context.weapons()->getWeaponStrategy("RocketLauncher");

            desc::Weapon description;
            description.name = "rocket launcher";
            description.fireRate = 1.0;

            weapon.assign<c::Weapon>(description);
            weapon.assign<c::WeaponStrategy>(strategy);

            auto equipment = object->getComponent<c::Equipment>();
            equipment->setPrimaryItem(weapon); // @todo: удалять/перемещать предыдущее (например в инвентарь)
        }
    }

    void World::forcePush(game::WorldObject *object, const vec3 &direction, const float &force) {
        auto entity = object->getEntity();
        context.physic()->applyForce(entity, direction * force);
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

    void World::setupRenderMesh(ex::Entity &entity) {
        context.renderer()->setupModel(entity);
    }

    void World::applyObjectAsset(game::WorldObject *object, assets::Model *asset) {
        ex::Entity entity = object->getEntity();

        if (entity.has_component<c::Mesh>()) {
            entity.remove<c::Mesh>();
        }

        entity.assign<c::Mesh>(asset->getMeshes());

        context.physic()->computeBoundingBox(object);

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
        context.renderer()->setRenderer(renderer);
    }
}
