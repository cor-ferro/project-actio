#include "story_importer.h"

namespace game {
    game::Story StoryImporter::import(const Resource::File &file) {
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
                    std::vector<std::string> scripts = scriptsNode.as<std::vector<std::string>>();

                    for (const auto &scriptName : scripts) {
                        chapterBuilder.addScript(scriptName);
                    }
                }


                /**
                 * Load textures
                 */
                YAML::Node texturesNode = chapterNode["textures"];
                if (texturesNode.IsSequence()) {
                    std::vector<std::string> textures = texturesNode.as<std::vector<std::string>>();

                    for (const auto &texturePath : textures) {
                        chapterBuilder.addTexture(texturePath);
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
                        resource.textures = node["textures"].as<std::vector<std::string>>();
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

        game::assets::Loader assetsLoader(chapterPath.string());

        const std::vector<std::string> &scriptNames = chapter->getScripts();
        for (const auto &scriptPath : scriptNames) {
            assets::Resource *resource = assetsLoader.load(scriptPath);

            if (resource != nullptr) {
                assets->addScript(resource);
            }
        }

        const std::vector<std::string> &textureNames = chapter->getTextures();
        for (const auto &texturePath : textureNames) {
            assets::Resource *resource = assetsLoader.load(texturePath);

            if (resource != nullptr) {
                assets->addTexture(resource);
            }
        }

        return true;
    }
}