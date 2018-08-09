#ifndef ACTIO_LIB_ASSETS_H
#define ACTIO_LIB_ASSETS_H

#include <vector>
#include <map>
#include <fstream>
#include <streambuf>
#include <cassert>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include "assets_resource.h"
#include "assets_types.h"
#include "console.h"
#include "data_loader.h"

class Assets {
public:
    typedef size_t Id;

    static const Id InvalidId = 0;

    Assets() = default;

    ~Assets();

    std::map<Id, assets::Script> &getScripts();

    std::map<Id, assets::Shader> &getShaders();

    std::map<Id, assets::Model> &getModels();

    std::map<Id, assets::Texture> &getTextures();

    std::map<Id, assets::Material> &getMaterials();

    void addScript(assets::Resource *resource);

    assets::Texture *addTexture(assets::Resource *resource);

    assets::Texture *addTexture(const std::string &name, assets::Resource *resource);

    assets::Texture *getTexture(std::string name);

    assets::Texture *getTexture(Id id);

    assets::Material *addMaterial(Material *material);

    assets::Material *addMaterial(const std::string &name, Material *material);

    assets::Material *getMaterial(const Id id);

    assets::Material *getMaterial(const std::string &name);

    assets::Material *createMaterial(const std::string &name);

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

    Id idCounter = 1;

    Id genId();
};

#endif //ACTIO_LIB_ASSETS_H
