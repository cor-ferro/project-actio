#ifndef ACTIO_LIB_ASSETS_H
#define ACTIO_LIB_ASSETS_H

#include <vector>
#include <map>
#include <fstream>
#include <streambuf>
#include <cassert>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <assimp/scene.h>
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
    using Id = size_t;

    static const Id InvalidId = 0;

    template<typename T>
    class Set {
    public:
        std::map<Id, T> items;
        std::map<std::string, Id> names;

        ~Set() {
            clear();
        }

        void clear() {
            items.clear();
            names.clear();
        }
    };

    enum class DefaultImage {
        Diffuse,
        Normal,
        Specular,
        Height
    };

private:
    Set<assets::Shader> shaders;
    Set<assets::Image> textures;
    Set<assets::Model> models;
    Set<assets::Script> scripts;
    Set<assets::Material> materials;

    Id idCounter = 1;

    Id genId();

    const AppPaths &appPaths;

    Path getMaterialTexture(aiMaterial *const material, const aiTextureType& texType);

public:
    explicit Assets(const AppPaths &appPaths);

    ~Assets();

    template<typename T>
    T *get(const std::string &name, Set<T> &set);

    template<typename T>
    T *get(const Assets::Id &id, Set<T> &set);

    template<typename T>
    T *add(Resource *resource, Set<T> &set);

    template<typename T>
    T *add(const std::string &name, Resource *resource, Set<T> &set);


    Set<assets::Script> &getScripts();

    Set<assets::Shader> &getShaders();

    Set<assets::Model> &getModels();

    Set<assets::Image> &getImages();

    Set<assets::Material> &getMaterials();


    assets::Model *getModel(const std::string &name);

    assets::Model *getModel(const Id &id);

    assets::Image *getImage(const std::string &name);

    assets::Image *getImage(const Id &id);

    assets::Image *getImage(const DefaultImage& defaultImageType);

    assets::Material *getMaterial(const Id &id);

    assets::Material *getMaterial(const std::string &name);



    assets::Model *addModel(Resource *resource, const std::unordered_map<std::string, std::string> &options);

    assets::Model *addModel(const std::string &name, Resource *resource, const std::unordered_map<std::string, std::string> &options);

    assets::Script *addScript(Resource *resource);

    assets::Script *addScript(const std::string &name, Resource *resource);

    assets::Image *addImage(Resource *resource);

    assets::Image *addImage(const std::string &name, Resource *resource);

    assets::Material *addMaterial(Material *material);

    assets::Material *addMaterial(const std::string &name, Material *material);

    void loadModel(assets::Model *model);

    std::shared_ptr<::Material> createMaterial(const std::string &name);

    void free();
};

#endif //ACTIO_LIB_ASSETS_H
