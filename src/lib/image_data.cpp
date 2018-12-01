#include "image_data.h"


ImageData::ImageData(const ImageData &other) {
    data_ = other.data_;
    width = other.width;
    height = other.height;
    size = other.size;
    format = other.format;
    stride = other.stride;
};

ImageData::ImageData(RawData *data, Res width, Res height, Format format)
        : width(width)
        , height(height)
        , format(format)
        , stride(ImageData::componentSize(format)) {
    auto dataSize = static_cast<size_t>(width * height * stride);

    set(data, dataSize);
    calcSize();
}

ImageData::~ImageData() {
    free();
}

bool ImageData::isReady() {
    return data_ != nullptr;
};


void ImageData::calcSize() {
    stride = ImageData::componentSize(format);
    size = width * height * sizeof(RawData);
};

void ImageData::set(RawData *data, size_t size) {
    free();

    auto *newData = new RawData[size];
    for (size_t i = 0; i < size; i++) {
        newData[i] = data[i];
    }

    length = size;
    data_ = newData;
};

void ImageData::free() {
    if (data_ != nullptr) {
        delete[] data_;
        data_ = nullptr;
    }
};

ImageData::RawData *ImageData::get() const {
    return data_;
};

void ImageData::initData(Res newWidth, Res newHeight, Format newFormat) {
    width = newWidth;
    height = newHeight;
    format = newFormat;
}

const ImageData::Res ImageData::getWidth() {
    return width;
}

const ImageData::Res ImageData::getHeight() {
    return height;
}

const ImageData::Format ImageData::getFormat() {
    return format;
}

int ImageData::componentSize(Format format) {
    switch (format) {
        case GL_RG:
            return 2;

        case GL_RED:
        case GL_RGB:
        case GL_BGR:
            return 3;

        case GL_RGBA:
        case GL_BGRA:
        case 6409: // GL_LUMINANCE
            return 4;

        case GL_RG_INTEGER:
            return 2;

        case GL_RED_INTEGER:
        case GL_RGB_INTEGER:
        case GL_BGR_INTEGER:
            return 3;

        case GL_RGBA_INTEGER:
        case GL_BGRA_INTEGER:
            return 4;
        default:
            console::warn("unknown image format %s", format);
            return 3;
    }
}

const int ImageData::getStride() {
    return stride;
}

const size_t ImageData::getLength() {
    return length;
}