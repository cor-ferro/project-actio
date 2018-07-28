#include "assets_types.h"
#include "image_parser.h"

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
    Model::Model(assets::Resource *resource) : BaseAsset(resource) {};
}