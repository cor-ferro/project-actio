#include "assets.h"
#include "assets_loader.h"
#include "image_parser.h"
#include "console.h"
#include "filesystem_resource.h"

Assets::Assets(const AppPaths &appPaths) : appPaths(appPaths) {
    const Path defaultDiffuseImagePath = createPath(appPaths.resources, "default-diffuse.png");
    const Path defaultNormalImagePath = createPath(appPaths.resources, "default-normal.jpg");
    const Path defaultSpecularImagePath = createPath(appPaths.resources, "default-specular.png");
    const Path defaultHeightImagePath = createPath(appPaths.resources, "default-height.png");

    addImage("DefaultDiffuseTexture", new FilesystemResource(defaultDiffuseImagePath));
    addImage("DefaultNormalTexture", new FilesystemResource(defaultNormalImagePath));
    addImage("DefaultSpecularTexture", new FilesystemResource(defaultSpecularImagePath));
    addImage("DefaultHeightTexture", new FilesystemResource(defaultHeightImagePath));
}

Assets::~Assets() {
    console::warn("destroy assets");
    free();
}

template<typename T>
T* Assets::get(const std::string &name, Set<T>& set) {
    auto nameIt = set.names.find(name);

    if (nameIt == set.names.end()) {
        return nullptr;
    }

    auto idIt = set.items.find(nameIt->second);
    if (idIt == set.items.end()) {
        return nullptr;
    }

    return &idIt->second;
}

template<typename T>
T* Assets::get(const Assets::Id& id, Set<T>& set) {
    auto it = set.items.find(id);

    if (it == set.items.end()) {
        return nullptr;
    }

    return &it->second;
}

template<typename T>
T* Assets::add(Resource *resource, Set<T>& set) {
    add("", resource, set);
}

template<typename T>
T* Assets::add(const std::string &name, Resource *resource, Set<T>& set) {
    Assets::Id id = genId();

    if (!name.empty()) {
        auto it = set.names.find(name);
        if (it != set.names.end()) {
            console::warn("texture %s already exists in assets", name);
            return nullptr;
        }
    }

    set.names.insert({name, id});
    auto insert = set.items.emplace(std::piecewise_construct, std::forward_as_tuple(id), std::forward_as_tuple(resource));

    if (insert.second) {
        return &insert.first->second;
    }

    return nullptr;
}

Assets::Set<assets::Script> &Assets::getScripts() {
    return scripts;
};

Assets::Set<assets::Shader> &Assets::getShaders() {
    return shaders;
};

Assets::Set<assets::Model> &Assets::getModels() {
    return models;
};

Assets::Set<assets::Image> &Assets::getImages() {
    return textures;
};

Assets::Set<assets::Material> &Assets::getMaterials() {
    return materials;
};

assets::Script *Assets::addScript(Resource *resource) {
    return addScript("", resource);
}

assets::Script *Assets::addScript(const std::string &name, Resource *resource) {
    return add(name, resource, scripts);
}

assets::Image *Assets::addImage(Resource *resource) {
    return addImage("", resource);
}

assets::Image * Assets::addImage(const std::string &name, Resource *resource) {
    return add(name, resource, textures);
}

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

assets::Image *Assets::getImage(const std::string &name) {
    return get<assets::Image>(name, textures);
}

assets::Image *Assets::getImage(const Assets::Id &id) {
    return get<assets::Image>(id, textures);
}

assets::Image *Assets::getImage(const Assets::DefaultImage & defaultImageType) {
    switch (defaultImageType) {
        case Assets::DefaultImage::Diffuse: return getImage("DefaultDiffuseTexture");
        case Assets::DefaultImage::Normal: return getImage("DefaultNormalTexture");
        case Assets::DefaultImage::Specular: return getImage("DefaultSpecularTexture");
        case Assets::DefaultImage::Height: return getImage("DefaultHeightTexture");
    }
}

assets::Material *Assets::getMaterial(const std::string &name) {
    return get<assets::Material>(name, materials);
}

assets::Material *Assets::getMaterial(const Assets::Id& id) {
    return get<assets::Material>(id, materials);
}

assets::Material *Assets::addMaterial(Material *material) {
    return addMaterial("", material);
}

assets::Material *Assets::addMaterial(const std::string &name, Material *material) {
    Id id = genId();

    if (!name.empty()) {
        auto it = materials.names.find(name);
        if (it != materials.names.end()) {
            console::warn("material %s already exists", name);
            return nullptr;
        }
    }

    materials.names.insert({name, id});
    auto insert = materials.items.emplace(std::piecewise_construct, std::forward_as_tuple(id), std::forward_as_tuple(material));

    if (insert.second) {
        return &insert.first->second;
    }

    return nullptr;
}

assets::Model *Assets::addModel(Resource *resource, const std::unordered_map<std::string, std::string> &options) {
    return addModel("", resource, options);
}

assets::Model *Assets::addModel(const std::string &name, Resource *resource, const std::unordered_map<std::string, std::string> &options) {
    Id id = genId();

    if (!name.empty()) {
        auto it = models.names.find(name);
        if (it != models.names.end()) {
            console::warn("model %s already exists", name);
            return nullptr;
        }
    }

    models.names.insert({name, id});
    auto insert = models.items.emplace(std::piecewise_construct, std::forward_as_tuple(id), std::forward_as_tuple(resource, options));

    if (insert.second) {
        loadModel(&insert.first->second);

        return &insert.first->second;
    }

    return nullptr;
}

assets::Model *Assets::getModel(const std::string& name) {
    return get<assets::Model>(name, models);
}

assets::Model *Assets::getModel(const Assets::Id& id) {
    return get<assets::Model>(id, models);
}

void Assets::loadModel(assets::Model *model) {
//    const aiScene *scene = model->getScene();
//
//    const unsigned int numMeshes = scene->mNumMeshes;
//    for (unsigned int i = 0; i < numMeshes; i++) {
//        aiMesh *mesh = scene->mMeshes[i];
//        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
//
//
//        {
////            Texture diffuseMap = Texture::Empty(Texture::Type::Diffuse, 0);
//            Path path = this->getMaterialTexture(material, aiTextureType_DIFFUSE);
//            Resource *resource = assetsLoader.createResource(path);
//            assets::Image *assetImage = addImage(path.string(), resource);
//        }
//
//        {
////            Texture specularMap = Texture::Empty(Texture::Type::Specular, 255);
//            Path path = this->getMaterialTexture(material, aiTextureType_SPECULAR);
//            Resource *resource = assetsLoader.createResource(path);
////            assets::Image *assetImage = addImage(path.string(), resource);
//        }
//
//        {
////            Texture heightMap = Texture::Empty(Texture::Type::Height, 0);
//            Path path = this->getMaterialTexture(material, aiTextureType_HEIGHT);
//            Resource *resource = assetsLoader.createResource(path);
////            assets::Image *assetImage = addImage(path.string(), resource);
//        }
//
//        {
////            Texture normalMap = Texture::Empty(Texture::Type::Normal, 0);
//            Path path = this->getMaterialTexture(material, aiTextureType_NORMALS);
//            Resource *resource = assetsLoader.createResource(path);
////            assets::Image *assetImage = addImage(path.string(), resource);
//        }
//
//        aiColor3D diffuseColor(0.f, 0.f, 0.f);
//        aiColor3D specularColor(0.f, 0.f, 0.f);
//        aiColor3D ambientColor(0.f, 0.f, 0.f);
//
//
//
//
//        std::string path = it.first;
//        Texture *textureMap = it.second;
//
//        std::shared_ptr<ImageData> image;
//
//        if (resource != nullptr) {
//            assetTexture = addImage(resource);
//        } else {
//            auto baseAssets = App::GetBaseAssets();
//            assetTexture = baseAssets->getTexture(*textureMap);
//        }
//
////        if (assetTexture != nullptr) {
////            image = assetTexture->getImage();
////            textureMap->setData(image);
////        }
//
////        material->setDiffuse(diffuseColor[0], diffuseColor[1], diffuseColor[2]);
////        material->setSpecular(specularColor[0], specularColor[1], specularColor[2]);
////        material->setAmbient(ambientColor[0], ambientColor[1], ambientColor[2]);
////        material->setWireframe(isWireframe);
////        material->setShininess(shininessValue);
////
////        material->setDiffuseMap(diffuseMap);
////        material->setSpecularMap(specularMap);
////        material->setNormalMap(normalMap);
////        material->setHeightMap(heightMap);
//    }
}

Path Assets::getMaterialTexture(aiMaterial *const material, const aiTextureType& texType) {
    unsigned int countTextures = material->GetTextureCount(texType);

    std::string texturePath;

//    if (countTextures > 0) {
//        aiString tex;
//        material->GetTexture(texType, 0, &tex);
//
//        texturePath = tex.C_Str();
//
//        if (!file_exists(texturePath)) {
//            texturePath = defaultTexture;
//        }
//    } else {
//        texturePath = defaultTexture;
//    }

    return texturePath;
}

std::shared_ptr<::Material> Assets::createMaterial(const std::string &name) {
    auto asset = getMaterial(name);

    if (asset != nullptr) {
        const ::Material& material = *asset->getMaterial().get();

        return std::make_shared<::Material>(::Material(material));
    }

    return std::shared_ptr<::Material>();
}
