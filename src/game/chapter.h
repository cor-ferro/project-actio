//
// Created by demitriy on 7/7/18.
//

#ifndef ACTIO_CHAPTER_H
#define ACTIO_CHAPTER_H

#include <string>
#include <unordered_map>
#include "script.h"
#include "../lib/path.h"

namespace game {
    class ChapterBuilder;

    class Chapter {
        friend ChapterBuilder;

    public:
        typedef std::unordered_map<std::string, std::string> ResourceOptions;

        struct ResourceScript {
            std::string name;
            std::string path;
        };

        struct ResourceTexture {
            std::string name;
            std::string path;
        };

        struct ResourceShader {
            std::string name;
            std::vector<std::string> paths;
        };

        struct ResourceModel {
            std::string name;
            std::string path;
            ResourceOptions options;
        };

        struct ResourceMaterial {
            std::string name;
            std::string diffuseTexture;
            std::string normalTexture;
            std::string specularTexture;
            std::string heightTexture;
            std::string shader;
            ResourceOptions options;
        };

        const std::string getName();

        const std::vector<ResourceScript> &getScripts();

        const std::vector<ResourceTexture> &getTextures();

        const std::vector<ResourceShader> &getShaders();

        const std::vector<ResourceModel> &getModels();

        const std::vector<ResourceMaterial> &getMaterials();

    private:
        std::string name;
        std::vector<ResourceScript> scripts;
        std::vector<ResourceTexture> textures;
        std::vector<ResourceShader> shaders;
        std::vector<ResourceModel> models;
        std::vector<ResourceMaterial> materials;
    };

    class ChapterBuilder {
    public:
        explicit ChapterBuilder(Chapter &chapter);

        void setName(const std::string &name);

        void addScript(const Chapter::ResourceScript &resource);

        void addTexture(const Chapter::ResourceTexture &resource);

        void addModel(const Chapter::ResourceModel &resource);

        void addShader(const Chapter::ResourceShader &resource);

        void addMaterial(const Chapter::ResourceMaterial &resource);
    private:
        Chapter &chapter;
    };
}

#endif //ACTIO_CHAPTER_H
