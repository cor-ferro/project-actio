#include "assets_texture.h"
#include "image_parser.h"

namespace assets {
    /**
     * -------------------- Texture --------------------
     */
    Image::Image(Resource *resource) : BaseAsset(resource) {};

    std::shared_ptr<ImageData> Image::getImage() {
        if (!image) {
            resource->load();

            ImageParser parser;
            void const* imageResourceData = resource->get();

            if (imageResourceData != nullptr) {
                std::shared_ptr<ImageData> newImage = parser.parse(
                        static_cast<const char *>(imageResourceData),
                        resource->getSize()
                );

                image = newImage;
            }
        }

        return image;
    }
}