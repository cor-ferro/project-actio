//
// Created by demitriy on 7/22/18.
//

#ifndef ACTIO_ASSETS_TYPES_H
#define ACTIO_ASSETS_TYPES_H

#include <vector>
#include "assets_resource.h"
#include "../lib/path.h"
#include "../lib/image_data.h"
#include "../lib/text_set.h"

namespace game {
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
            explicit Model(assets::Resource *resource);
        };


        class Texture : BaseAsset {
        public:
            explicit Texture(assets::Resource *resource);

        private:
            std::shared_ptr<ImageData> image;
        };


        class Material {
            std::vector<Texture> textures;
            Shader shader;
        };
    }
}

#endif //ACTIO_ASSETS_TYPES_H
