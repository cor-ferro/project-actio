#ifndef ACTIO_IMAGE_DATA_H
#define ACTIO_IMAGE_DATA_H

#include <cstddef>
#include <memory>
#include <glad/glad.h>
#include "console.h"

class ImageData {
public:
    typedef unsigned char RawData;
    typedef int Res;
    typedef int Format;

    ImageData() = default;

    ImageData(const ImageData &other);

    ImageData(RawData *data, Res width, Res height, Format format);

    ~ImageData();

    bool isReady();

    void calcSize();

    void set(RawData *data, size_t size);

    void free();

    RawData *get() const;

    static int componentSize(int format);

    void initData(Res width, Res height, Format format);

    Res getWidth();

    Res getHeight();

    Format getFormat();

protected:
    RawData *data_ = nullptr;
    Res width = 0;
    Res height = 0;
    size_t size = 0;
    Format format = 0;
};

#endif //ACTIO_IMAGE_DATA_H
