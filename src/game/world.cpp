#include <algorithm>
#include <thread>
#include "world.h"
#include "systems/physic.h"
#include "systems/movement.h"
#include "systems/render.h"
#include "systems/ball_shoot.h"
#include "systems/animations.h"
#include "components/transform.h"
#include "components/state.h"
#include "components/renderable.h"
#include "../math/Box3.h"
#include "world_importer.h"

namespace game {
    World::World()
            : name(""), time(0) {

    }

    void World::update(TimeDelta dt) {
        systems.update<game::systems::Movement>(dt);
        systems.update<game::systems::Physic>(dt);
        systems.update<game::systems::Animations>(dt);
        systems.update<game::systems::BallShoot>(dt);
    }

    void World::render(TimeDelta dt) {
        systems.update<systems::Render>(dt);
    }

    void World::setupRenderer(renderer::Renderer *renderer) {
        systems.add<game::systems::Render>(renderer);
    }

    void World::setupMovement(InputHandler *ih) {
        systems.add<game::systems::Movement>(ih);
    }

    void World::setup() {
        systems.add<game::systems::Animations>();
        systems.add<game::systems::Physic>();
        systems.add<game::systems::BallShoot>();
        systems.configure();

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

    void World::initGroudPlane() {
        Material::Phong material;
        material.setWireframe(true);
        material.setDiffuse(0.0f, 0.0f, 0.5f);

        Geometry geometry = Geometry::Plane(20, 20, 10, 10);
        Mesh *mesh = Mesh::Create(geometry, material);

        entityx::Entity entity = entities.create();

        entity.assign<components::Transform>(
                vec3(0.0f),
                glm::angleAxis(glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f)),
                vec3(1.0f)
        );
        entity.assign<components::Model>(mesh);

        events.emit<game::events::RenderSetupMesh>(entity, mesh);
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
}
