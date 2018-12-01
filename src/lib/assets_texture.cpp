#include "assets_texture.h"
#include "image_parser.h"

namespace assets {
    /**
     * -------------------- Texture --------------------
     */
    Texture::Texture(Resource *resource) : BaseAsset(resource) {};

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
}