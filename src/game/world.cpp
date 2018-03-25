#include <algorithm>
#include "world.h"
#include "systems/physic.h"
#include "systems/movement.h"
#include "systems/render.h"
#include "components/position.h"
#include "components/transform.h"
#include "components/state.h"
#include "components/renderable.h"
#include "components/physic.h"
#include "components/controlled.h"
#include "components/model.h"

namespace game {
    World::World()
            : name(""), time(0) {

    }

    void World::update(TimeDelta dt) {
        systems.update<game::systems::Physic>(dt);
        systems.update<game::systems::Movement>(dt);
    }

    void World::render(TimeDelta dt) {
        systems.update<systems::Render>(dt);
    }

    void World::setupRenderer(renderer::Renderer *renderer) {
        systems.add<game::systems::Render>(renderer);
    }

    void World::setup() {
        systems.add<game::systems::Movement>();
        systems.add<game::systems::Physic>();
        systems.configure();
    }

    inline void World::spawn(Character *character) {
        spawn(character, vec3(0.0f, 0.0f, 0.0f));
    }

    void World::spawn(Character *character, vec3 position) {
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

    void World::add() {

    }

    void World::remove() {

    }

    void World::initFromFile(const Resource::File &file) {
        console::info("load world from file: %s", file.getPath());

        IniLoader iniLoader;
        iniLoader.defineSection("world", {"models", "lights", "skybox", "camera"});
        iniLoader.defineSection("model", {"File", "FlipUv", "Animation", "Position", "Rotation", "Scale", "Player"});
        iniLoader.defineSection("camera", {"Type", "Position", "Target", "AspectRatio", "Fov", "Near", "Far"});
        iniLoader.defineSection("light", {"Type", "Ambient", "Diffuse", "Specular", "Position", "Constant", "Linear",
                                          "Quadratic", "Direction"});
        iniLoader.defineSection("skybox",
                                {"PositiveX", "NegativeX", "PositiveY", "NegativeY", "PositiveZ", "NegativeZ"});
        iniLoader.load(file.getPath());

        std::shared_ptr<game::systems::Physic> physicSystem = systems.system<game::systems::Physic>();
        physx::PxControllerManager *const controllerManager = physicSystem->getControllerManager();

        const IniLoader::Section *world = iniLoader.getSection("world");

        std::vector<std::string> worldModels;

        if (world != nullptr) {
            world->getField("models", worldModels, ' ');
        }

        const std::vector<IniLoader::Section> *models = iniLoader.getSections("model");
        if (models != nullptr) {
            for (auto it = models->begin(); it != models->end(); ++it) {
                const IniLoader::Section &section = (*it);

                vec3 pos, scale;
                vec4 rot;
                std::string filePath, animation;
                bool flipUv, player, isInWorld;

                section.getFieldVec<vec3>("Position", pos);
                section.getFieldVec<vec4>("Rotation", rot);
                section.getFieldVec<vec3>("Scale", scale, 1.0f);

                section.getField("File", filePath);
                section.getField("FlipUv", flipUv);
                section.getField("Animation", animation);
                section.getField("Player", player);

                isInWorld = std::find(worldModels.begin(), worldModels.end(), section.name) != worldModels.end();

                Material::Phong material;
                material.setWireframe(true);
                material.setDiffuse(0.0f, 1.0f, 0.0f);

                Geometry geometry = Geometry::Box();
                Mesh *mesh = Mesh::Create(geometry, material);

                entityx::Entity entity = entities.create();

                entity.assign<components::Transform>(pos, rot, scale);
                entity.assign<components::Model>(mesh);

                if (player) {
                    entity.assign<components::Controlled>(controllerManager);
                }

                if (isInWorld) {
                    console::info("is in world: %s", section.name);
                    entity.assign<components::Renderable>();
                }
            }
        }
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
