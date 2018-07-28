#include <algorithm>
#include <thread>
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
#include "components/char_items.h"
#include "desc/light_spot.h"
#include "strategies/weapons/default.h"
#include "strategies/weapons/rocket_launcher.h"
#include "world_importer.h"
#include "../math/Box3.h"

namespace game {
    World::World() : name("") {

    }

    void World::setupRenderer(renderer::Renderer *renderer) {
        systems.add<game::systems::Render>(&context, renderer);
    }

    void World::destroyRenderer() {
        renderer->destroy();
    }

    void World::setup() {
//        entities.each<c::Model, c::Skin, c::Character, c::CharItems>([](ex::Entity, c::Model &, c::Skin &, c::Character &, c::CharItems &) {});

        systems.add<systems::Input>(&context);
        systems.add<systems::Camera>(&context);
        systems.add<systems::CharControl>(&context);
        systems.add<systems::CharacterControl>(&context); // deprecated
        systems.add<systems::Animations>(&context);
        systems.add<systems::Physic>(&context);
        systems.add<systems::BallShot>(&context);
        systems.add<systems::LightHelpers>(&context);
        systems.add<systems::DayTime>(&context);
        systems.add<systems::Sky>(&context);
        systems.add<systems::Weapons>(&context, this);

        systems.configure();

        camera = systems.system<game::systems::Camera>();
        physic = systems.system<game::systems::Physic>();
        weapons = systems.system<game::systems::Weapons>();
        renderer = systems.system<game::systems::Render>();

        registerWeapon(new strategy::WeaponsDefault());
        registerWeapon(new strategy::WeaponsRocketLauncher());

        physic->postConfigure(events);

        initGroundPlane();

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
            PROFILE(systemProfiler, script->getName(), script->evalUpdate());
        }

        // post update
        PROFILE(systemProfiler, "Animations", systems.update<game::systems::Animations>(dt));
        PROFILE(systemProfiler, "LightHelpers", systems.update<game::systems::LightHelpers>(dt));
    }

    void World::render(ex::TimeDelta dt) {
        systems.update<systems::Render>(dt);
    }

    inline void World::spawn(Character *character) {
        spawn(character, vec3(0.0f, 0.0f, 0.0f));
    }

    void World::spawn(Character *character, vec3 position) {
        entityx::Entity entity = entities.create();
        entity.assign<components::State>();
        entity.assign<components::Renderable>();
        entity.assign<components::Transform>(position);

        // WorldObject<Character> object;
        // object.object = character;
        // object.position = position;
        // object.frozen = false;
        // object.hidden = false;
    }

    World::Character World::createCharacter(Resource::Assimp *resource) {
        return createCharacter("", resource);
    }

    World::Character World::createCharacter(std::string name, Resource::Assimp *resource) {
        ex::Entity entity = entities.create();

        World::Character character(entity, name);

        character.setup(resource, assets.get());

        events.emit<game::events::RenderSetupModel>(entity);
        events.emit<events::CharacterCreate>();

        return character;
    }

    void World::removeCharacter(World::Character character) {
        ex::Entity entity = character.getEntity();

        events.emit<events::CharacterRemove>();

        entity.destroy();
    }

    World::StaticObject World::createStaticObject(Mesh *mesh) {
        ex::Entity entity = entities.create();

        World::StaticObject object(entity, mesh);

        events.emit<game::events::RenderSetupModel>(entity);

        return object;
    }

    void World::removeStaticObject(World::StaticObject object) {
        ex::Entity entity = object.getEntity();

        entity.destroy();
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

    void World::add() {

    }

    void World::remove() {

    }

    void World::initGroundPlane() {
        Mesh *mesh = Mesh::Create();

        GeometryPrimitive::Plane(mesh->geometry, 20, 20, 10, 10);
//        mesh->material.setWireframe(true);
        mesh->material->setDiffuse(0.0f, 1.0f, 0.0f);

        entityx::Entity entity = entities.create();

        entity.assign<components::Transform>(
                vec3(0.0f),
                glm::angleAxis(glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f)),
                vec3(1.0f)
        );
        entity.assign<components::Model>(mesh);

        events.emit<events::RenderSetupModel>(entity);
    }

    void World::destroy() {
        unloadScripts();

        entities.reset();

        console::info("world %s destroyed", name);
    }

    void World::setRenderSize(renderer::ScreenSize width, renderer::ScreenSize height) {
        events.emit<events::RenderResize>(width, height);
    }

    void World::setInput(int place, InputHandler *ih) {
        console::info(" SET INPUT %i", ih);
        context.input = ih;
        events.emit<events::InputSetHandler>(ih, place);
    }

    void World::removeInput(systems::Input::InputPlace place) {

    }

    const game::Context &World::getContext() {
        return context;
    }

    World::Character World::getUserControlCharacter() {
        ex::Entity charEntity;

        entities.each<components::UserControl>([&charEntity](ex::Entity entity, components::UserControl &control) {
            charEntity = entity;
        });

        World::Character character = World::Character::Restore(charEntity);

        return character;
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

        components::LightSpot light;
        light.setAmbient(lightDescription.ambient);
        light.setDiffuse(lightDescription.diffuse);
        light.setSpecular(lightDescription.specular);
        light.setPosition(lightDescription.position);
        light.setDirection(lightDescription.direction);
        light.setAttenuation(lightDescription.constant, lightDescription.linear, lightDescription.quadratic);
        light.setCutoff(lightDescription.cutOff, lightDescription.outerCutOff);

        entity.assign<components::LightSpot>(light);
        entity.assign<components::Transform>(lightDescription.position);

        events.emit<events::LightAdd>(entity);
    }

    void World::removeLight(entityx::Entity entity) {
        // @todo: check light components
        events.emit<events::LightRemove>(entity);

        entity.destroy();
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
}
