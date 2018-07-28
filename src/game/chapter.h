//
// Created by demitriy on 7/7/18.
//

#ifndef ACTIO_CHAPTER_H
#define ACTIO_CHAPTER_H

#include <string>
#include "script.h"
#include "../lib/path.h"

namespace game {
    class ChapterBuilder;

    class Chapter {
        friend ChapterBuilder;

    public:
        typedef std::map<std::string, std::string> ResourceOptions;

        struct ResourceShader {
            std::string name;
            std::vector<std::string> paths;
        };

        struct ResourceModel {
            std::string name;
            std::string path;
            ResourceOptions options;

            bool hasOption(std::string key) const {
                return options.find(key) != options.end();
            }

            std::string getOption(std::string key) const {
                auto it = options.find(key);

                if (it != options.end()) {
                    return it->second;
                }

                return "";
            }
        };

        struct ResourceMaterial {
            std::string name;
            std::vector<std::string> textures;
            std::string shader;
            ResourceOptions options;
        };

        const std::string getName();

        const std::vector<std::string> &getScripts();

        const std::vector<std::string> &getTextures();

        const std::vector<ResourceShader> &getShaders();

        const std::vector<ResourceModel> &getModels();

        const std::vector<ResourceMaterial> &getMaterials();

    private:
        std::string name;
        std::vector<std::string> scripts;
        std::vector<std::string> textures;
        std::vector<ResourceShader> shaders;
        std::vector<ResourceModel> models;
        std::vector<ResourceMaterial> materials;
    };

    class ChapterBuilder {
    public:
        explicit ChapterBuilder(Chapter &chapter);

        void setName(const std::string &name);

        void addScript(std::string str);

        void addTexture(std::string path);

        void addModel(const Chapter::ResourceModel &resource);

        void addShader(const Chapter::ResourceShader &resource);

        void addMaterial(const Chapter::ResourceMaterial &resource);
    private:
        Chapter &chapter;
    };
}

#endif //ACTIO_CHAPTER_H
