#ifndef ACTIO_ASSETS_TEXTURE_H
#define ACTIO_ASSETS_TEXTURE_H

#include "assets_base.h"
#include "resource.h"
#include "image_data.h"

namespace assets {
    class Texture : public BaseAsset {
    public:
        explicit Texture(Resource *resource);

        std::shared_ptr<ImageData> getImage();

    private:
        std::shared_ptr<ImageData> image;
    };
}

#endif //ACTIO_ASSETS_TEXTURE_H
