#include <assimp/include/assimp/scene.h>
#include <assimp/include/assimp/postprocess.h>
#include <assimp/include/assimp/Importer.hpp>
#include "story_importer.h"
#include "../app/app.h"
#include "../resources/resources.h"

namespace game {
    game::Story StoryImporter::import(const resources::File &file) {
        console::info("-----------------------------------------------");

        rootPath = file.getPath();
        rootPath = rootPath.parent_path();

        game::Story story;
        game::StoryBuilder storyBuilder(story);

        YAML::Node baseNode = YAML::LoadFile(file.getPath());
        YAML::Node storyNode = baseNode["story"];

        const std::string name = storyNode["name"].as<std::string>();
        const std::vector<std::string> chapters = storyNode["chapters"].as<std::vector<std::string>>();

        console::info("story: %s", name);

        storyBuilder.setName(name);

        for (const auto &chapterDirName : chapters) {
            console::info("chapter: %s", chapterDirName);

            game::Chapter chapter;
            game::ChapterBuilder chapterBuilder(chapter);


            boost::filesystem::path chapterDir(file.getPath());
            chapterDir = chapterDir.parent_path();
            chapterDir /= chapterDirName;

            boost::filesystem::path chapterFile = chapterDir;
            chapterFile /= "chapter.yaml";

            console::info("chapter dir: %s", chapterDir);
            if (boost::filesystem::exists(chapterFile)) {
                YAML::Node chapterBaseNode = YAML::LoadFile(chapterFile.string());
                YAML::Node chapterNode = chapterBaseNode["chapter"];

                const std::string chapterName = chapterNode["name"].as<std::string>();
                chapterBuilder.setName(chapterName);


                /**
                 * Load scripts
                 */
                YAML::Node scriptsNode = chapterNode["scripts"];
                if (scriptsNode.IsSequence()) {
                    for (auto node : scriptsNode) {
                        Chapter::ResourceScript resource;

                        if (node.IsScalar()) {
                            resource.name = node.as<std::string>();
                            resource.path = node.as<std::string>();
                        } else if (node.IsMap()) {
                            resource.name = node["name"].as<std::string>();
                            resource.path = node["path"].as<std::string>();
                        } else {
                            console::warn("unknown script node type");
                        }

                        chapterBuilder.addScript(resource);
                    }
                }


                /**
                 * Load textures
                 */
                YAML::Node texturesNode = chapterNode["textures"];
                if (texturesNode.IsSequence()) {
                    for (auto node : texturesNode) {
                        Chapter::ResourceTexture resource;

                        if (node.IsScalar()) {
                            resource.name = node.as<std::string>();
                            resource.path = node.as<std::string>();
                        } else if (node.IsMap()) {
                            resource.name = node["name"].as<std::string>();
                            resource.path = node["path"].as<std::string>();
                        } else {
                            console::warn("unknown texture node type");
                        }

                        chapterBuilder.addTexture(resource);
                    }
                }


                /**
                 * Load models
                 */
                YAML::Node modelsNode = chapterNode["models"];
                if (modelsNode.IsSequence()) {
                    for (auto node : modelsNode) {
                        Chapter::ResourceModel resource;
                        resource.name = node["name"].as<std::string>();
                        resource.path = node["file"].as<std::string>();

                        for (auto it : node) {
                            resource.options[it.first.as<std::string>()] = it.second.as<std::string>();
                        }

                        chapterBuilder.addModel(resource);
                    }
                }


                /**
                 * Load materials
                 */
                YAML::Node materialsNode = chapterNode["materials"];
                if (materialsNode.IsSequence()) {
                    for (auto node : materialsNode) {
                        Chapter::ResourceMaterial resource;
                        resource.name = node["name"].as<std::string>();

                        auto diffuseTextureNode = node["diffuseTexture"];
                        auto normalTextureNode = node["normalTexture"];
                        auto specularTextureNode = node["specularTexture"];
                        auto heightTextureNode = node["heightTexture"];

                        resource.diffuseTexture = diffuseTextureNode.IsScalar() ? diffuseTextureNode.as<std::string>() : "";
                        resource.normalTexture = normalTextureNode.IsScalar() ? normalTextureNode.as<std::string>() : "";
                        resource.specularTexture = specularTextureNode.IsScalar() ? specularTextureNode.as<std::string>() : "";
                        resource.heightTexture = heightTextureNode.IsScalar() ? heightTextureNode.as<std::string>() : "";
                        resource.shader = node["shader"].as<std::string>();

                        chapterBuilder.addMaterial(resource);
                    }
                }


                /**
                 * Load shaders
                 */
                YAML::Node shadersNode = chapterNode["shaders"];
                if (shadersNode.IsSequence()) {
                    for (auto node : shadersNode) {
                        Chapter::ResourceShader resource;
                        resource.name = node["name"].as<std::string>();
                        resource.paths = node["files"].as<std::vector<std::string>>();

                        chapterBuilder.addShader(resource);
                    }
                }

                storyBuilder.addChapter(chapter);
            } else {
                console::warn("chapter %s not exists", chapterDirName);
            }
        }

        console::info("-----------------------------------------------");

        return story;
    }

    void StoryImporter::loadScripts(const std::vector<std::string> &scripts, const boost::filesystem::path &fromPath) {}

    void StoryImporter::loadTextures() {}

    void StoryImporter::loadShaders() {}

    void StoryImporter::loadModels() {}

    void StoryImporter::loadMaterials() {}

    bool StoryImporter::loadChapterAssets(Story &story, const std::string &chapterName, Assets *assets) {
        Chapter *chapter = story.getChapter(chapterName);

        if (chapter == nullptr) {
            return false;
        }

        boost::filesystem::path chapterPath = rootPath;
        chapterPath /= chapterName;

        assets::Loader assetsLoader(chapterPath.string());

        const std::vector<Chapter::ResourceScript> &scripts = chapter->getScripts();
        for (const auto &chapterResource : scripts) {
            Resource *resource = assetsLoader.createResource(chapterResource.path);

            if (resource != nullptr) {
                assets->addScript(resource);
            }
        }

        const std::vector<Chapter::ResourceTexture> &textures = chapter->getTextures();
        for (const auto &chapterResource : textures) {
            Resource *resource = assetsLoader.createResource(chapterResource.path);

            if (resource != nullptr) {
                assets->addImage(chapterResource.name, resource);
            }
        }

        const std::vector<Chapter::ResourceModel> &models = chapter->getModels();
        for (const auto &modelResource : models) {
            Resource *resource = assetsLoader.createResource(modelResource.path);

            assets->addModel(modelResource.name, resource, modelResource.options);
        }

        const std::vector<Chapter::ResourceMaterial>& materials = chapter->getMaterials();
        for (const auto &materialResource : materials) {
            auto *material = new Material();
            Texture diffuseMap = Texture::Empty(Texture::Type::Diffuse, 0);
            Texture specularMap = Texture::Empty(Texture::Type::Specular, 0);
            Texture heightMap = Texture::Empty(Texture::Type::Height, 0);
            Texture normalMap = Texture::Empty(Texture::Type::Normal, 255);

            std::vector<std::pair<std::string, Texture*>> texturesToLoad;

            if (!materialResource.diffuseTexture.empty()) {
                texturesToLoad.push_back({ materialResource.diffuseTexture, &diffuseMap });
            }

            if (!materialResource.normalTexture.empty()) {
                texturesToLoad.push_back({ materialResource.normalTexture, &normalMap });
            }

            if (!materialResource.specularTexture.empty()) {
                texturesToLoad.push_back({ materialResource.specularTexture, &specularMap });
            }

            if (!materialResource.heightTexture.empty()) {
                texturesToLoad.push_back({ materialResource.heightTexture, &heightMap });
            }

            for (auto &it : texturesToLoad) {
                std::string textureName = it.first;
                Texture *texture = it.second;

                assets::Image *assetImage = assets->getImage(materialResource.diffuseTexture);
                std::shared_ptr<ImageData> image;

                if (assetImage != nullptr) {
                    image = assetImage->getImage();
                } else {
                    Chapter::ResourceTexture chapterTextureResource;
                    chapterTextureResource.path = materialResource.diffuseTexture;
                    chapterTextureResource.name = chapterTextureResource.path;

                    Resource *resource = assetsLoader.createResource(chapterTextureResource.path);

                    if (resource != nullptr) {
                        assetImage = assets->addImage(chapterTextureResource.name, resource);
                        image = assetImage->getImage();
                    } else {
                        assetImage = assets->getImage(Assets::DefaultImage::Diffuse);
                        image = assetImage->getImage();
                    }
                }

                texture->setData(image);
            }

            material->setDiffuseMap(diffuseMap);
            material->setSpecularMap(specularMap);
            material->setHeightMap(heightMap);
            material->setNormalMap(normalMap);

            assets->addMaterial(materialResource.name, material);
        }

        return true;
    }
}