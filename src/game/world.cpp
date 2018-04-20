#include <algorithm>
#include <thread>
#include "world.h"
#include "systems/physic.h"
#include "systems/camera.h"
#include "systems/render.h"
#include "systems/ball_shoot.h"
#include "systems/animations.h"
#include "systems/character_control.h"
#include "systems/input.h"
#include "components/transform.h"
#include "components/state.h"
#include "components/renderable.h"
#include "../math/Box3.h"
#include "world_importer.h"
#include "events/camera_look_at.h"
#include "events/light_add.h"
#include "events/light_remove.h"
#include "systems/light_helpers.h"
#include "components/light_point.h"
#include "components/light_directional.h"
#include "desc/light_spot.h"
#include "systems/day_time.h"

namespace game {
    World::World() : name("") {}

    void World::update(ex::TimeDelta dt) {
        // pre update
        systems.update<game::systems::Input>(dt);
        systems.update<game::systems::Camera>(dt);
        systems.update<game::systems::CharacterControl>(dt);
        systems.update<game::systems::Physic>(dt);
        systems.update<game::systems::DayTime>(dt);

        // main update
        systems.update<game::systems::BallShoot>(dt);

        // post update
        systems.update<game::systems::Animations>(dt);
        systems.update<game::systems::LightHelpers>(dt);
    }

    void World::render(ex::TimeDelta dt) {
        systems.update<systems::Render>(dt);
    }

    void World::setupRenderer(renderer::Renderer *renderer) {
        systems.add<game::systems::Render>(&context, renderer);
    }

    void World::setupInput(InputHandler *ih) {
        systems.add<game::systems::Input>(&context, ih);
        systems.add<game::systems::Camera>(&context, ih);
    }

    void World::setup() {
        systems.add<game::systems::CharacterControl>(&context);
        systems.add<game::systems::Animations>(&context);
        systems.add<game::systems::Physic>(&context);
        systems.add<game::systems::BallShoot>(&context);
        systems.add<game::systems::LightHelpers>(&context);
        systems.add<game::systems::DayTime>(&context);

        systems.configure();

        camera = systems.system<game::systems::Camera>();

        physic = systems.system<game::systems::Physic>();
        physic->setDrawDebug(true);
        physic->postConfigure(events);

        camera->postConfigure(events);

        console::info("world configure end");

        initGroudPlane();
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

    void World::add() {

    }

    void World::remove() {

    }

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

    void World::initGroudPlane() {
        Mesh *mesh = Mesh::Create();

        GeometryPrimitive::Plane(mesh->geometry, 20, 20, 10, 10);
//        mesh->material.setWireframe(true);
        mesh->material.setDiffuse(0.0f, 1.0f, 0.0f);

        entityx::Entity entity = entities.create();

        entity.assign<components::Transform>(
                vec3(0.0f),
                glm::angleAxis(glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f)),
                vec3(1.0f)
        );
        entity.assign<components::Model>(mesh);

        events.emit<events::RenderSetupMesh>(entity, mesh);
    }

    void World::destroy() {
        for (WorldObject<Character> &object : characters) {
            if (object.object != nullptr) {
                delete object.object;
                object.object = nullptr;
            }
        }

        characters.erase(characters.begin(), characters.end());

        console::info("world %s destroyed", name);
    }

    void World::setPhysicsDebug(bool value) {
        physic->setDrawDebug(value);
    }

    void World::setLightDebug(bool value) {
        events.emit<events::LightHelperShow>(value);
    }

    void World::setCameraSettings(float fov, float aspect, float near, float far) {
        camera->setSettings(fov, aspect, near, far);
    }

//    void World::getCameraSettings() {
//
//    }

    void World::setGravity(vec3 dir) {
        physic->setSceneGravity(dir);
    }

    void World::cameraLookAt(vec3 target) {
        camera->lookAt(target);
    }
}
