#include "chapter.h"

namespace game {
    const std::string Chapter::getName() {
        return name;
    }

    const std::vector<std::string> &Chapter::getScripts() {
        return scripts;
    }

    const std::vector<std::string> &Chapter::getTextures() {
        return textures;
    }

    const std::vector<Chapter::ResourceShader> &Chapter::getShaders() {
        return shaders;
    }

    const std::vector<Chapter::ResourceModel>& Chapter::getModels() {
        return models;
    }

    const std::vector<Chapter::ResourceMaterial>& Chapter::getMaterials() {
        return materials;
    }


    ChapterBuilder::ChapterBuilder(Chapter &chapter) : chapter(chapter) {}

    void ChapterBuilder::setName(const std::string &name) {
        chapter.name = name;
    }

    void ChapterBuilder::addScript(std::string str) {
        chapter.scripts.emplace_back(str);
    }

    void ChapterBuilder::addTexture(std::string path) {
        chapter.textures.emplace_back(path);
    }

    void ChapterBuilder::addModel(const Chapter::ResourceModel &resource) {
        chapter.models.push_back(resource);
    }

    void ChapterBuilder::addShader(const Chapter::ResourceShader &resource) {
        chapter.shaders.push_back(resource);
    };

    void ChapterBuilder::addMaterial(const Chapter::ResourceMaterial &resource) {
        chapter.materials.push_back(resource);
    }
}