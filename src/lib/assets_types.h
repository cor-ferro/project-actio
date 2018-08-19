//
// Created by demitriy on 7/22/18.
//

#ifndef ACTIO_LIB_ASSETS_TYPES_H
#define ACTIO_LIB_ASSETS_TYPES_H

#include <vector>
#include <memory>
#include <unordered_map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include "assets_resource.h"
#include "path.h"
#include "image_data.h"
#include "text_set.h"
#include "../core/material.h"
#include "../core/model.h"
#include "../core/skin.h"
#include "../core/mesh.h"

namespace assets {
    class BaseAsset {
    public:
        explicit BaseAsset(assets::Resource *resource);

        ~BaseAsset();

        const assets::Resource *getResource();

    protected:
        bool isLoaded = false;
        assets::Resource *resource = nullptr;
    };

    class Script : public BaseAsset {
    public:
        explicit Script(assets::Resource *resource);

        void setContent(Resource::Content newContent);

        Resource::Content getContent();

    private:
        Resource::Content content;
    };

    class Shader : public BaseAsset {
    public:
        explicit Shader(assets::Resource *resource);

        ~Shader() = default;

        const char *const getVertexContent() const;

        const char *const getFragmentContent() const;

        const char *const getGeometryContent() const;

    private:
        TextSet data;
    };

    class Texture : public BaseAsset {
    public:
        explicit Texture(assets::Resource *resource);

        std::shared_ptr<ImageData> getImage();

    private:
        std::shared_ptr<ImageData> image;
    };

    class Material : public BaseAsset {
    public:
        explicit Material(::Material *material);

        const std::shared_ptr<::Material> &getMaterial() const;

    private:
        std::shared_ptr<::Material> material;

        std::vector<std::shared_ptr<Texture>> textures;
    };

    class Model : public BaseAsset {
    public:
        typedef std::unordered_map<std::string, std::string> Options;

        explicit Model(assets::Resource *resource, const Options &options);

        ~Model();

        void load();

        bool hasOption(const std::string &key) const;

        const std::string getOption(const std::string &key) const;

        bool isLoaded();

        void markLoaded();

        void markUnLoaded();

        const aiScene *getScene();

        const ::Resource::Assimp *getAiResource();

        void addMesh(std::shared_ptr<::Mesh> &mesh);

        const std::vector<std::shared_ptr<::Mesh>> &getMeshes();

    private:
        Assimp::Importer importer;
        const aiScene *scene = nullptr;
        ::Resource::Assimp *assimpResource = nullptr;

        bool loaded = false;
        Options options;

        std::vector<std::shared_ptr<Material>> materials;
        std::vector<std::shared_ptr<::Mesh>> meshes;
    };
}

#endif //ACTIO_LIB_ASSETS_TYPES_H
