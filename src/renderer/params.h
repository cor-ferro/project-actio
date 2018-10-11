//
// Created by demitriy on 10/6/18.
//

#ifndef ACTIO_PARAMS_H
#define ACTIO_PARAMS_H

namespace renderer {
    typedef unsigned int Dimension;

    struct Params {
    private:
        Dimension width = 0;
        Dimension height = 0;
        float aspectRatio = 0.0f;

    public:
        void setWidth(const Dimension &newWidth);

        void setHeight(const Dimension &newHeight);

        void setAspectRatio(const float &newAspectRatio);

        const Dimension &getWidth() const;

        const Dimension &getHeight() const;

        const float &getAspectRatio() const;
    };
}

#endif //ACTIO_PARAMS_H
