#include "assets.h"
#include "assets_resource.h"
#include "../lib/console.h"

namespace game {
    Assets::~Assets()  {
        console::warn("destroy assets");
        free();
    }

    void Assets::addScript(assets::Resource *resource) {
        scripts.emplace(std::piecewise_construct,
                        std::forward_as_tuple(genId()),
                        std::forward_as_tuple(resource)
        );
    }

    void Assets::addTexture(assets::Resource *resource) {
        textures.emplace(std::piecewise_construct,
                         std::forward_as_tuple(genId()),
                         std::forward_as_tuple(resource)
        );
    }

    const std::map<Assets::Id, assets::Script> &Assets::getScripts() {
        return scripts;
    };

    const std::map<Assets::Id, assets::Shader> &Assets::getShaders() {
        return shaders;
    };

    const std::map<Assets::Id, assets::Model> &Assets::getModels() {
        return models;
    };

    const std::map<Assets::Id, assets::Texture> &Assets::getTextures() {
        return textures;
    };

    const std::map<Assets::Id, assets::Material> &Assets::getMaterials() {
        return materials;
    };

    void Assets::free() {
        shaders.clear();
        textures.clear();
        models.clear();
        scripts.clear();
        materials.clear();
    }

    Assets::Id Assets::genId() {
        return ++idCounter;
    }
}