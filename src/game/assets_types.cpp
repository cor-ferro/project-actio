#include "assets_types.h"
#include "../lib/image_parser.h"

namespace game {
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

        void Script::setContent(char *newContent) {
            content.reset(newContent, std::default_delete<char[]>());
        }

        const char *Script::getContent() {
            if (!content) {
                setContent(resource->get());
            }

            return content.get();
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
        Texture::Texture(assets::Resource *resource) : BaseAsset(resource) {
            ImageParser parser;

            image.reset(new ImageData());

            char *imageResourceData = resource->get();

            if (imageResourceData != nullptr) {
                ImageData *ptr = image.get();
                *ptr = parser.parse(resource->get(), resource->getSize());
            }
        };


        /**
         * -------------------- Model --------------------
         */
        Model::Model(assets::Resource *resource) : BaseAsset(resource) {};
    }
}