//
// Created by demitriy on 7/22/18.
//

#ifndef ACTIO_LIB_ASSETS_TYPES_H
#define ACTIO_LIB_ASSETS_TYPES_H

#include <vector>
#include <memory>
#include <unordered_map>
#include "assets_resource.h"
#include "path.h"
#include "image_data.h"
#include "text_set.h"
#include "../core/material.h"

namespace assets {
    class BaseAsset {
    public:
        explicit BaseAsset(assets::Resource *resource);

        ~BaseAsset();

    protected:
        bool isLoaded = false;
        assets::Resource *resource = nullptr;
    };


    class Script : BaseAsset {
    public:
        explicit Script(assets::Resource *resource);

        void setContent(Resource::Content newContent);

        Resource::Content getContent();

    private:
        Resource::Content content;
    };


    class Shader : BaseAsset {
    public:
        explicit Shader(assets::Resource *resource);

        ~Shader() = default;

        const char *const getVertexContent() const;

        const char *const getFragmentContent() const;

        const char *const getGeometryContent() const;

    private:
        TextSet data;
    };


    class Model : BaseAsset {
    public:
        typedef std::unordered_map<std::string, std::string> Options;

        explicit Model(assets::Resource *resource, const Options &options);

        void init();

        bool hasOption(const std::string &key) const;

        const std::string getOption(const std::string &key) const;

    private:
        Options options;
    };


    class Texture : BaseAsset {
    public:
        explicit Texture(assets::Resource *resource);

        std::shared_ptr<ImageData> getImage();

    private:
        std::shared_ptr<ImageData> image;
    };


    class Material : BaseAsset {
    public:
        explicit Material(::Material *material);

        const std::shared_ptr<::Material> &getMaterial() const;

    private:
        std::shared_ptr<::Material> material;
    };
}

#endif //ACTIO_LIB_ASSETS_TYPES_H
