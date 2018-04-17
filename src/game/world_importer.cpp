#include "world_importer.h"
#include "systems/physic.h"
#include "../lib/ini_loader.h"
#include "events/render_setup_mesh.h"
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
                "Geometry", "Size", "File", "FlipUv", "Animation", "Position", "Rotation", "Scale", "Player", "Physic"
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

        if (worldSection != nullptr) {
            worldSection->getField("models", worldModels, ' ');
        }

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

                entityx::Entity entity = world->entities.create();

                rotQuat = glm::angleAxis(glm::radians(rot.w), vec3(rot.x, rot.y, rot.z));
                entity.assign<components::Transform>(pos, rotQuat, scale);

                if (geometryType == "file" && isInWorld) {
                    Path p = createPath(std::string(RESOURCE_DIR), filePath); // move RESOURCE_DIR to app
                    components::Model::File modelFile(p.string());

                    modelFile.name = section.name;
                    modelFile.flipUv = flipUv;
                    modelFile.animation = animation;

                    Assimp::Importer importer;

                    std::string pFile = modelFile.file.getPath();
                    unsigned int flags = aiProcessPreset_TargetRealtime_Quality
                                         | aiProcess_GenSmoothNormals
                                         | aiProcess_Triangulate
                                         | aiProcess_CalcTangentSpace;

                    if (flipUv) {
                        flags |= aiProcess_FlipUVs;
                    }

                    const aiScene *scene = importer.ReadFile(pFile, flags);
                    const std::unique_ptr<Resource::Assimp> assimpResource(new Resource::Assimp(scene, pFile));

                    if (scene) {
                        entityx::ComponentHandle<components::Model> model = entity.assign<components::Model>(
                                assimpResource.get());

                        if (isPhysic) {}

                        if (assimpResource->hasAnimations()) {
                            entityx::ComponentHandle<components::Skin> skin = entity.assign<components::Skin>(assimpResource.get());

                            auto nodeIndexes = skin->getNodeIndexes();
                            model->reindexMeshBones(nodeIndexes);

                            if (!animation.empty()) {
                                skin->setCurrentAnimation(animation);
                            }
                        }

                        for (Mesh *mesh : model->getMeshes()) {
                            world->events.emit<game::events::RenderSetupMesh>(entity, mesh);
                        }

                    } else {
                        console::err("failed model loading %s", importer.GetErrorString());
                    }

                    importer.FreeScene();

                } else if (geometryType == "box") {
                    vec3 size;
                    section.getFieldVec<vec3>("Size", size, vec3(1.0f));

                    Mesh *mesh = Mesh::Create();
                    GeometryPrimitive::Box(mesh->geometry, size.x, size.y, size.z);
                    mesh->material.setDiffuse(0.0f, 1.0f, 0.0f);

                    entity.assign<components::Model>(mesh);

                    if (isInWorld) {
                        if (isPhysic) {
                            world->events.emit<events::PhysicCreateBox>(entity, size.x, size.y, size.z);
                        }
                        world->events.emit<events::RenderSetupMesh>(entity, mesh);
                    }
                }

                if (player) {
                    world->events.emit<events::SetupControlled>(entity);
                }
            }
        }

        if (lights != nullptr) {
            for (const auto &section : *lights) {
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

                    world->addLight(desc);
                } else if (type == "directional") {
                    desc::LightDirectionalDesc desc;

                    section.getFieldVec<vec3>("Ambient", desc.ambient);
                    section.getFieldVec<vec3>("Diffuse", desc.diffuse);
                    section.getFieldVec<vec3>("Specular", desc.specular);
                    section.getFieldVec<vec3>("Direction", desc.direction);

                    world->addLight(desc);
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

                    world->addLight(desc);
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