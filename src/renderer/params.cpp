#include "params.h"

namespace renderer {
    void Params::setWidth(const Dimension &newWidth) {
        width = newWidth;
        setAspectRatio((float)width / (float)height);
    }

    void Params::setHeight(const Dimension &newHeight) {
        height = newHeight;
        setAspectRatio((float)width / (float)height);
    }

    void Params::setAspectRatio(const float &newAspectRatio) {
        aspectRatio = newAspectRatio;
    }

    const Dimension &Params::getWidth() const {
        return width;
    }

    const Dimension &Params::getHeight() const {
        return height;
    }

    const float &Params::getAspectRatio() const {
        return aspectRatio;
    }
}