#include "assets_types.h"
#include "image_parser.h"
#include "../resources/assimp_resource.h"
#include <assimp/include/assimp/postprocess.h>
#include "../core/model_builder.h"

namespace assets {
    /**
     * -------------------- base --------------------
     */
    BaseAsset::BaseAsset(assets::Resource *resource) : resource(resource) {};

    BaseAsset::~BaseAsset() {
        if (resource != nullptr) {
            delete resource;
            resource = nullptr;
        }
    }

    const assets::Resource *BaseAsset::getResource() {
        return resource;
    }


    /**
     * -------------------- Script --------------------
     */
    Script::Script(assets::Resource *resource) : BaseAsset(resource) {
        content = nullptr;
    };

    void Script::setContent(Resource::Content newContent) {
        content = newContent;
    }

    Resource::Content Script::getContent() {
        if (!content) {
            setContent(resource->get());
        }

        return content;
    }


    /**
     * -------------------- Shader --------------------
     */
    Shader::Shader(assets::Resource *resource) : BaseAsset(resource) {};

    const char *const Shader::getVertexContent() const {
        return data.getRaw("vertex");
    }

    const char *const Shader::getFragmentContent() const {
        return data.getRaw("fragment");
    }

    const char *const Shader::getGeometryContent() const {
        return data.getRaw("geometry");
    }


    /**
     * -------------------- Texture --------------------
     */
    Texture::Texture(assets::Resource *resource) : BaseAsset(resource) {};

    std::shared_ptr<ImageData> Texture::getImage() {
        if (!image) {
            resource->load();

            ImageParser parser;
            Resource::Content imageResourceData = resource->get();

            if (imageResourceData != nullptr) {
                std::shared_ptr<ImageData> newImage = parser.parse(imageResourceData->c_str(),
                                                                   imageResourceData->length());
                image = newImage;
            }
        }

        return image;
    }


    /**
     * -------------------- Model --------------------
     */
    Model::Model(assets::Resource *resource, const Options &options) : BaseAsset(resource), options(options) {}

    Model::~Model() {
        if (assimpResource != nullptr) {
            delete assimpResource;
        }

        if (scene != nullptr) {
            importer.FreeScene();
        }
    }

    bool Model::hasOption(const std::string &key) const  {
        return options.find(key) != options.end();
    }

    const std::string Model::getOption(const std::string &key) const {
        auto it = options.find(key);

        if (it != options.end()) {
            return it->second;
        }

        return "";
    }

    bool Model::isLoaded() {
        return loaded;
    }

    void Model::markLoaded() {
        loaded = true;
    }

    void Model::markUnLoaded() {
        loaded = false;
    }

    void Model::load() {
        bool flipUv = getOption("flipUv") == "true";

        unsigned int flags = aiProcessPreset_TargetRealtime_Quality
                             | aiProcess_GenSmoothNormals
                             | aiProcess_Triangulate
                             | aiProcess_CalcTangentSpace;

        if (flipUv) {
            flags |= aiProcess_FlipUVs;
        }

        const assets::Resource *resource = getResource();
        const Path &path = resource->getPath();

        scene = importer.ReadFile(path.string().c_str(), flags);

        if (scene != nullptr) {
            assimpResource = new ::Resource::Assimp(scene, path.string());
        }
    }

    const aiScene *Model::getScene() {
        if (!isLoaded()) {
            load();
            markLoaded();
        }

        return scene;
    }

    const std::vector<std::shared_ptr<::Mesh>> &Model::getMeshes() {
        return meshes;
    }

    void Model::addMesh(std::shared_ptr<::Mesh> &mesh) {
        meshes.push_back(mesh);
    }

    const ::Resource::Assimp *Model::getAiResource() {
        return assimpResource;
    }


    /**
     * -------------------- Material --------------------
     */
    Material::Material(::Material *material) : BaseAsset(nullptr) {
        Material::material.reset(material);
    }

    const std::shared_ptr<::Material> &Material::getMaterial() const {
        return material;
    }
}