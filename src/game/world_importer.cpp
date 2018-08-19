#include "world_importer.h"
#include "systems/physic.h"
#include "../lib/ini_loader.h"
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
#include "components/char_items.h"

namespace game {
    WorldImporter::WorldImporter(World *world) : world(world) {}

    void WorldImporter::import(const Resource::File &file) {
        console::info("load world from file: %s", file.getPath());
        importFromIniFile(file);
    }

    void WorldImporter::importFromIniFile(const Resource::File &file) {
        IniLoader iniLoader;

        std::vector<std::string> worldFields = {"models", "lights", "skybox", "camera"};
        std::vector<std::string> cameraFields = {"Type", "Position", "Target", "AspectRatio", "Fov", "Near", "Far"};
        std::vector<std::string> skyboxFields = {
                "PositiveX", "NegativeX", "PositiveY", "NegativeY", "PositiveZ", "NegativeZ"
        };
        std::vector<std::string> lightFields = {
                "Type", "Ambient", "Diffuse", "Specular", "Position", "Constant", "Linear", "Quadratic", "Direction",
                "CutOff", "OuterCutOff"
        };
        std::vector<std::string> modelFields = {
                "Geometry", "Size", "File", "FlipUv", "Animation", "Position", "Rotation", "Scale", "Control", "Physic"
        };

        iniLoader.defineSection("world", worldFields);
        iniLoader.defineSection("model", modelFields);
        iniLoader.defineSection("camera", cameraFields);
        iniLoader.defineSection("light", lightFields);
        iniLoader.defineSection("skybox", skyboxFields);

        iniLoader.load(file.getPath());

        const IniLoader::Section *worldSection = iniLoader.getSection("world");
        const IniLoader::Section *cameraSection = iniLoader.getSection("camera");
        const std::vector<IniLoader::Section> *models = iniLoader.getSections("model");
        const std::vector<IniLoader::Section> *lights = iniLoader.getSections("light");

        std::vector<std::string> worldModels;
        std::vector<std::string> worldLights;

        if (worldSection != nullptr) {
            worldSection->getField("models", worldModels, ' ');
            worldSection->getField("lights", worldLights, ' ');
        }

        if (models != nullptr) {
            for (const auto &section : *models) {
                bool isInWorld = std::find(worldModels.begin(), worldModels.end(), section.name) != worldModels.end();

                if (!isInWorld) {
                    continue;
                }

                vec3 pos, scale;
                vec4 rot;
                quat rotQuat;
                std::string geometryType, filePath, animation;
                bool flipUv, control, isPhysic;

                section.getFieldVec<vec3>("Position", pos, vec3(0.0f));
                section.getFieldVec<vec4>("Rotation", rot);
                section.getFieldVec<vec3>("Scale", scale, vec3(1.0f));

                section.getField("Geometry", geometryType, "file");
                section.getField("Physic", isPhysic);

                section.getField("File", filePath);
                section.getField("FlipUv", flipUv);
                section.getField("Animation", animation);
                section.getField("Control", control);

                rotQuat = glm::angleAxis(glm::radians(rot.w), vec3(rot.x, rot.y, rot.z));
            }
        }

        if (lights != nullptr) {
            for (const auto &section : *lights) {
                bool isInWorld = std::find(worldLights.begin(), worldLights.end(), section.name) != worldLights.end();
                entityx::Entity entity = world->entities.create();

                std::string type;
                section.getField("Type", type);

                if (type == "point") {
                    desc::LightPointDesc desc;

                    section.getFieldVec<vec3>("Ambient", desc.ambient);
                    section.getFieldVec<vec3>("Diffuse", desc.diffuse);
                    section.getFieldVec<vec3>("Specular", desc.specular);
                    section.getFieldVec<vec3>("Position", desc.position);
                    section.getField("Constant", desc.constant);
                    section.getField("Linear", desc.linear);
                    section.getField("Quadratic", desc.quadratic);

                    if (isInWorld) world->addLight(desc);
                } else if (type == "directional") {
                    desc::LightDirectionalDesc desc;

                    section.getFieldVec<vec3>("Ambient", desc.ambient);
                    section.getFieldVec<vec3>("Diffuse", desc.diffuse);
                    section.getFieldVec<vec3>("Specular", desc.specular);
                    section.getFieldVec<vec3>("Direction", desc.direction);

                    if (isInWorld) world->addLight(desc);
                } else if (type == "spot") {
                    desc::LightSpotDesc desc;

                    section.getFieldVec<vec3>("Ambient", desc.ambient);
                    section.getFieldVec<vec3>("Diffuse", desc.diffuse);
                    section.getFieldVec<vec3>("Specular", desc.specular);
                    section.getFieldVec<vec3>("Position", desc.position);
                    section.getFieldVec<vec3>("Direction", desc.direction);
                    section.getField("Constant", desc.constant);
                    section.getField("Linear", desc.linear);
                    section.getField("Quadratic", desc.quadratic);
                    section.getField("CutOff", desc.cutOff);
                    section.getField("OuterCutOff", desc.outerCutOff);

                    if (isInWorld) world->addLight(desc);
                } else {
                    console::warn("unknown light type");
                }
            }
        }

        if (cameraSection != nullptr) {
            entityx::Entity entity = world->entities.create();

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

                entityx::ComponentHandle<components::Camera> camera = entity.assign<components::Camera>(fov, aspect, near, far);
                entity.assign<components::Transform>(position);
                entity.assign<components::Target>(target);

                camera->setPosition(position);
                world->cameraLookAt(vec3(0.0f));
            } else if (type == "ortho") {
                console::warn("unsupported camera type: %s", "ortho");
            } else {
                console::warn("unsupported camera type");
            }
        }
    }
}