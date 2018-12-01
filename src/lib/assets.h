#ifndef ACTIO_LIB_ASSETS_H
#define ACTIO_LIB_ASSETS_H

#include <vector>
#include <map>
#include <fstream>
#include <streambuf>
#include <cassert>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include "resource.h"
#include "console.h"
#include "data_loader.h"
#include "../core/texture.h"
#include "assets_script.h"
#include "assets_shader.h"
#include "assets_texture.h"
#include "assets_material.h"
#include "assets_model.h"
#include "app_paths.h"

class Assets {
public:
    typedef size_t Id;

    static const Id InvalidId = 0;

    explicit Assets(const AppPaths& appPaths) : appPaths(appPaths) {};

    ~Assets();

    std::map<Id, assets::Script> &getScripts();

    std::map<Id, assets::Shader> &getShaders();

    std::map<Id, assets::Model> &getModels();

    std::map<Id, assets::Texture> &getTextures();

    std::map<Id, assets::Material> &getMaterials();

    assets::Model *addModel(Resource *resource, const std::unordered_map<std::string, std::string> &options);

    assets::Model *addModel(const std::string &name, Resource *resource, const std::unordered_map<std::string, std::string> &options);

    assets::Model *getModel(std::string name);

    assets::Model *getModel(Id id);

    void addScript(Resource *resource);

    assets::Texture *addTexture(Resource *resource);

    assets::Texture *addTexture(const std::string &name, Resource *resource);

    assets::Texture *getTexture(std::string name);

    assets::Texture *getTexture(Id id);

    assets::Material *addMaterial(Material *material);

    assets::Material *addMaterial(const std::string &name, Material *material);

    assets::Material *getMaterial(const Id& id);

    assets::Material *getMaterial(const std::string &name);

    assets::Material *createMaterial(const std::string &name);

    assets::Texture *getDefaultTexture(Texture &texture);

    void free();

    void loadDefaultResources();

private:
    std::map<Id, assets::Shader> shaders;
    std::map<Id, assets::Texture> textures;
    std::map<Id, assets::Model> models;
    std::map<Id, assets::Script> scripts;
    std::map<Id, assets::Material> materials;

    std::map<std::string, Id> textureNames; // @todo: use boost multi_index
    std::map<std::string, Id> materialNames; // @todo: use boost multi_index
    std::map<std::string, Id> modelNames; // @todo: use boost multi_index

    Id idCounter = 1;

    Id genId();

    const AppPaths& appPaths;
};

#endif //ACTIO_LIB_ASSETS_H
