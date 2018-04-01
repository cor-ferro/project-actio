#include <algorithm>
#include <thread>
#include "world.h"
#include "systems/physic.h"
#include "systems/movement.h"
#include "systems/render.h"
#include "systems/ball_shoot.h"
#include "components/position.h"
#include "components/transform.h"
#include "components/state.h"
#include "components/renderable.h"
#include "components/physic.h"
#include "components/controlled.h"
#include "components/model.h"
#include "components/light_directional.h"
#include "components/light_point.h"
#include "components/light_spot.h"
#include "components/camera.h"
#include "components/target.h"
#include "../math/Box3.h"

namespace game {
    World::World()
            : name(""), time(0) {

    }

    void World::update(TimeDelta dt) {
        systems.update<game::systems::Movement>(dt);
        systems.update<game::systems::Physic>(dt);
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
        iniLoader.defineSection("model", {"Geometry", "Size", "File", "FlipUv", "Animation", "Position", "Rotation", "Scale", "Player", "Physic"});
        iniLoader.defineSection("camera", {"Type", "Position", "Target", "AspectRatio", "Fov", "Near", "Far"});
        iniLoader.defineSection("light", {"Type", "Ambient", "Diffuse", "Specular", "Position", "Constant", "Linear",
                                          "Quadratic", "Direction", "CutOff", "OuterCutOff"});
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
            for (const auto &section : *models) {
                vec3 pos, scale;
                vec4 rot;
                quat rotQuat;
                std::string geometryType, filePath, animation;
                bool flipUv, player, isInWorld, isPhysic;

                section.getFieldVec<vec3>("Position", pos, vec3(0.0f));
                section.getFieldVec<vec4>("Rotation", rot);
                section.getFieldVec<vec3>("Scale", scale, vec3(1.0f));

                section.getField("Geometry", geometryType, "file");
                section.getField("Physic", isPhysic);

                section.getField("File", filePath);
                section.getField("FlipUv", flipUv);
                section.getField("Animation", animation);
                section.getField("Player", player);

                isInWorld = std::find(worldModels.begin(), worldModels.end(), section.name) != worldModels.end();

                entityx::Entity entity = entities.create();

                rotQuat = glm::angleAxis(glm::radians(rot.w), vec3(rot.x, rot.y, rot.z));
                entity.assign<components::Transform>(pos, rotQuat, scale);

                if (geometryType == "file" && isInWorld) {
                    console::info("flipUv: %i", flipUv);
                    components::Model::File modelFile(std::string("resources/") + filePath);
                    modelFile.name = section.name;
                    modelFile.flipUv = flipUv;
                    modelFile.animation = animation;

                    entityx::ComponentHandle<components::Model> model = entity.assign<components::Model>(modelFile);

                    if (isPhysic) {
//                        events.emit<events::PhysicCreateBox>(entity, size.x, size.y, size.z);
                        std::vector<Mesh*> meshes = model->getMeshes();
                        for (Mesh *mesh : meshes) {
//                            console::info("=============== bounding box");
//                            mesh->geometry.computeBoundingBox();
//                            const Math::Box3& meshBbox = mesh->geometry.getBoundingBox();
//
//                            events.emit<events::PhysicCreateBox>(entity, meshBbox.radius.x, meshBbox.radius.y, meshBbox.radius.z);
                        }
                    }

                    for (Mesh* mesh : model->getMeshes()) {
                        this->events.emit<game::events::RenderSetupMesh>(entity, mesh);
                    }

                } else if (geometryType == "box") {
                    vec3 size;
                    section.getFieldVec<vec3>("Size", size, vec3(1.0f));

                    Material::Phong material;
//                    material.setWireframe(true);
                    material.setDiffuse(0.0f, 1.0f, 0.0f);

                    Geometry geometry = Geometry::Box(size.x, size.y, size.z);
                    Mesh *mesh = Mesh::Create(geometry, material);
                    entity.assign<components::Model>(mesh);

                    if (isPhysic) {
                        events.emit<events::PhysicCreateBox>(entity, size.x, size.y, size.z);
                    }

                    if (isInWorld) {
                        events.emit<events::RenderSetupMesh>(entity, mesh);
                    }
                }

                if (player) {
                    entity.assign<components::Controlled>(controllerManager);
                }
            }
        }

        const std::vector<IniLoader::Section> *lights = iniLoader.getSections("light");
        if (models != nullptr) {
            for (const auto &section : *lights) {
                entityx::Entity entity = entities.create();

                std::string type;
                section.getField("Type", type);

                if (type == "point") {
                    vec3 ambient, diffuse, specular, position;
                    float constant, linear, quadratic;

                    section.getFieldVec<vec3>("Ambient", ambient);
                    section.getFieldVec<vec3>("Diffuse", diffuse);
                    section.getFieldVec<vec3>("Specular", specular);
                    section.getFieldVec<vec3>("Position", position);
                    section.getField("Constant", constant);
                    section.getField("Linear", linear);
                    section.getField("Quadratic", quadratic);

                    components::LightPoint light;
                    light.setPosition(position);
                    light.setAmbient(ambient);
                    light.setDiffuse(diffuse);
                    light.setSpecular(specular);
                    light.setAttenuation(constant, linear, quadratic);

                    console::info("ambient: %f, %f, %f", ambient.x, ambient.y, ambient.z);
                    console::info("diffuse: %f, %f, %f", diffuse.x, diffuse.y, diffuse.z);
                    console::info("specular: %f, %f, %f", specular.x, specular.y, specular.z);
                    console::info("position: %f, %f, %f", position.x, position.y, position.z);
                    console::info("constant: %f", constant);
                    console::info("linear: %f", linear);
                    console::info("quadratic: %f", quadratic);

                    entity.assign<components::LightPoint>(light);
                    entity.assign<components::Transform>(position);
                } else if (type == "directional") {
                    vec3 ambient, diffuse, specular, direction;

                    section.getFieldVec<vec3>("Ambient", ambient);
                    section.getFieldVec<vec3>("Diffuse", diffuse);
                    section.getFieldVec<vec3>("Specular", specular);
                    section.getFieldVec<vec3>("Direction", direction);

                    console::info("ambient: %f, %f, %f", ambient.x, ambient.y, ambient.z);
                    console::info("diffuse: %f, %f, %f", diffuse.x, diffuse.y, diffuse.z);
                    console::info("specular: %f, %f, %f", specular.x, specular.y, specular.z);
                    console::info("direction: %f, %f, %f", direction.x, direction.y, direction.z);

                    components::LightDirectional light;
                    light.setAmbient(ambient);
                    light.setDiffuse(diffuse);
                    light.setSpecular(specular);
                    light.setDirection(direction);

                    entity.assign<components::LightDirectional>(light);
                } else if (type == "spot") {
                    vec3 ambient, diffuse, specular, position, direction;
                    float constant, linear, quadratic, cutOff, outerCutOff;

                    section.getFieldVec<vec3>("Ambient", ambient);
                    section.getFieldVec<vec3>("Diffuse", diffuse);
                    section.getFieldVec<vec3>("Specular", specular);
                    section.getFieldVec<vec3>("Position", position);
                    section.getFieldVec<vec3>("Direction", direction);
                    section.getField("Constant", constant);
                    section.getField("Linear", linear);
                    section.getField("Quadratic", quadratic);
                    section.getField("CutOff", cutOff);
                    section.getField("OuterCutOff", outerCutOff);

                    components::LightSpot light;
                    light.setAmbient(ambient);
                    light.setDiffuse(diffuse);
                    light.setSpecular(specular);
                    light.setPosition(position);
                    light.setDirection(direction);
                    light.setAttenuation(constant, linear, quadratic);
                    light.setCutoff(cutOff, outerCutOff);

                    entity.assign<components::LightSpot>(light);
                    entity.assign<components::Transform>(position);
                } else {
                    console::warn("unknown light type");
                }
            }
        }

        const IniLoader::Section *cameraSection = iniLoader.getSection("camera");
        if (cameraSection != nullptr) {
            entityx::Entity entity = entities.create();

            std::string type;
            cameraSection->getField("Type", type);

            if (type == "perspective") {
                vec3 position, target;
                float aspect, fov, near, far;

                cameraSection->getFieldVec<vec3>("Position", position);
                cameraSection->getFieldVec<vec3>("Target", target);
                cameraSection->getField("AspectRatio", aspect);
                cameraSection->getField("Fov", fov);
                cameraSection->getField("Near", near);
                cameraSection->getField("Far", far);

                entity.assign<components::Camera>(fov, aspect, near, far);
                entity.assign<components::Transform>(position);
                entity.assign<components::Target>(target);
            } else if (type == "ortho") {
                console::warn("unsupported camera type: %s", "ortho");
            } else {
                console::warn("unsupported camera type");
            }
        }
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
