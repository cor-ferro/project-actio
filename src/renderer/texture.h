#ifndef ACTIO_TEXTURE_H
#define ACTIO_TEXTURE_H

#include <memory>
#include "../lib/image_data.h"

namespace renderer {
    class Texture {
    private:
        std::shared_ptr<ImageData> image;

        size_t offsetX = 0;
        size_t offsetY = 0;
        size_t tillingY = 0;
        size_t tillingX = 0;
    };
}

#endif //ACTIO_TEXTURE_H
