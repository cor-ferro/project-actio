#include "assets.h"
#include "assets_loader.h"
#include "image_parser.h"
#include "console.h"
#include "../app/app.h"

Assets::~Assets() {
    console::warn("destroy assets");
    free();
}

void Assets::addScript(Resource *resource) {
    scripts.emplace(std::piecewise_construct,
                    std::forward_as_tuple(genId()),
                    std::forward_as_tuple(resource)
    );
}

assets::Texture *Assets::addTexture(Resource *resource) {
    return addTexture("", resource);
}

assets::Texture * Assets::addTexture(const std::string &name, Resource *resource) {
    Id id = genId();

    if (!name.empty()) {
        auto it = textureNames.find(name);
        if (it != textureNames.end()) {
            console::warn("texture %s already exists in assets", name);
            return nullptr;
        }
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
    assets::Loader assetsLoader(appPaths.resources);

    const std::vector<std::pair<std::string, Path>> defaultTextures = {
        {"DefaultDiffuseTexture", "default-diffuse.png"},
        {"DefaultNormalTexture", "default-normal.jpg"},
        {"DefaultSpecularTexture", "default-specular.png"},
        {"DefaultHeightTexture", "default-height.png"}
    };

    for (const auto &it : defaultTextures) {
        const std::string &textureName = it.first;
        const Path &texturePath = it.second;

        Resource *defaultTextureResource = assetsLoader.createResource(texturePath);

        if (defaultTextureResource != nullptr) {
            addTexture(textureName, defaultTextureResource);
        } else {
            console::err("failed load default resource %s, %s", textureName, texturePath);
        }
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

assets::Material *Assets::getMaterial(const std::string &name) {
    auto nameIt = materialNames.find(name);
    if (nameIt == materialNames.end()) {
        return nullptr;
    }

    auto idIt = materials.find(nameIt->second);
    if (idIt == materials.end()) {
        return nullptr;
    }

    return &idIt->second;
}

assets::Material *Assets::getMaterial(const Assets::Id& id) {
    return nullptr;
}

assets::Material *Assets::addMaterial(Material *material) {
    return addMaterial("", material);
}

assets::Material *Assets::addMaterial(const std::string &name, Material *material) {
    Id id = genId();

    if (!name.empty()) {
        auto it = materialNames.find(name);
        if (it != materialNames.end()) {
            console::warn("material %s already exists", name);
            return nullptr;
        }
    }

    materialNames.insert({name, id});
    auto insert = materials.emplace(std::piecewise_construct, std::forward_as_tuple(id), std::forward_as_tuple(material));

    if (insert.second) {
        return &insert.first->second;
    }

    return nullptr;
}

assets::Material *Assets::createMaterial(const std::string &name) {
    assets::Material *currentAssetMaterial = getMaterial(name);

    if (!currentAssetMaterial) {
        return nullptr;
    }

    auto currentMaterial = currentAssetMaterial->getMaterial();
    auto material = new ::Material(*currentMaterial);
    auto assetMaterial = addMaterial(material);

    return assetMaterial;
}

assets::Texture *Assets::getDefaultTexture(Texture &texture) {
    switch (texture.type) {
        case Texture::Type::Diffuse: return getTexture("DefaultDiffuseTexture");
        case Texture::Type::Normal: return getTexture("DefaultNormalTexture");
        case Texture::Type::Specular: return getTexture("DefaultSpecularTexture");
        case Texture::Type::Height: return getTexture("DefaultHeightTexture");
        default: return getTexture("DefaultDiffuseTexture");
    }
}

assets::Model *Assets::addModel(Resource *resource, const std::unordered_map<std::string, std::string> &options) {
    return addModel("", resource, options);
}

assets::Model *Assets::addModel(const std::string &name, Resource *resource, const std::unordered_map<std::string, std::string> &options) {
    Id id = genId();

    if (!name.empty()) {
        auto it = modelNames.find(name);
        if (it != modelNames.end()) {
            console::warn("model %s already exists", name);
            return nullptr;
        }
    }

    modelNames.insert({name, id});
    auto insert = models.emplace(std::piecewise_construct, std::forward_as_tuple(id), std::forward_as_tuple(resource, options));

    if (insert.second) {
        return &insert.first->second;
    }

    return nullptr;
}

assets::Model *Assets::getModel(std::string name) {
    auto nameIt = modelNames.find(name);

    if (nameIt == modelNames.end()) {
        return nullptr;
    }

    auto idIt = models.find(nameIt->second);
    if (idIt == models.end()) {
        return nullptr;
    }

    return &idIt->second;
}

assets::Model *Assets::getModel(Assets::Id id) {
    auto it = models.find(id);

    if (it == models.end()) {
        return nullptr;
    }

    return &it->second;
}
