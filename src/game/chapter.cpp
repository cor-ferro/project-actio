#include "chapter.h"

namespace game {
    const std::string Chapter::getName() {
        return name;
    }

    const std::vector<Chapter::ResourceScript> &Chapter::getScripts() {
        return scripts;
    }

    const std::vector<Chapter::ResourceTexture> &Chapter::getTextures() {
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

    void ChapterBuilder::addScript(const Chapter::ResourceScript &resource) {
        chapter.scripts.push_back(resource);
    }

    void ChapterBuilder::addTexture(const Chapter::ResourceTexture &resource) {
        chapter.textures.push_back(resource);
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