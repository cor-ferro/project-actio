#include "assets.h"
#include "assets_resource.h"
#include "assets_loader.h"
#include "image_parser.h"
#include "console.h"
#include "../app/app.h"

Assets::~Assets() {
    console::warn("destroy assets");
    free();
}

void Assets::addScript(assets::Resource *resource) {
    scripts.emplace(std::piecewise_construct,
                    std::forward_as_tuple(genId()),
                    std::forward_as_tuple(resource)
    );
}

assets::Texture *Assets::addTexture(assets::Resource *resource) {
    return addTexture("", resource);
}

assets::Texture * Assets::addTexture(std::string name, assets::Resource *resource) {
    Id id = genId();

    if (name.empty()) {
        name = std::to_string(id);
    }

    auto it = textureNames.find(name);
    if (it != textureNames.end()) {
        console::warn("texture %s already exists in assets", name);
        return nullptr;
    }

    textureNames.insert({name, id});
    auto insert = textures.emplace(std::piecewise_construct, std::forward_as_tuple(id), std::forward_as_tuple(resource));

    if (insert.second) {
        return &insert.first->second;
    }

    return nullptr;
}

std::map<Assets::Id, assets::Script> &Assets::getScripts() {
    return scripts;
};

std::map<Assets::Id, assets::Shader> &Assets::getShaders() {
    return shaders;
};

std::map<Assets::Id, assets::Model> &Assets::getModels() {
    return models;
};

std::map<Assets::Id, assets::Texture> &Assets::getTextures() {
    return textures;
};

std::map<Assets::Id, assets::Material> &Assets::getMaterials() {
    return materials;
};

void Assets::free() {
    shaders.clear();
    textures.clear();
    models.clear();
    scripts.clear();
    materials.clear();
}

void Assets::loadDefaultResources() {
    assets::Loader assetsLoader(App::instance().resourcePath());

    assets::Resource *defaultTextureResource = assetsLoader.load(App::instance().resourcePath("default2.png"));

    if (defaultTextureResource != nullptr) {
        addTexture("DefaultTexture", defaultTextureResource);
    }
}

Assets::Id Assets::genId() {
    return ++idCounter;
}

assets::Texture *Assets::getTexture(std::string name) {
    auto nameIt = textureNames.find(name);

    if (nameIt == textureNames.end()) {
        return nullptr;
    }

    auto idIt = textures.find(nameIt->second);
    if (idIt == textures.end()) {
        return nullptr;
    }

    return &idIt->second;
}

assets::Texture *Assets::getTexture(Assets::Id id) {
    auto it = textures.find(id);

    if (it == textures.end()) {
        return nullptr;
    }

    return &it->second;
}
