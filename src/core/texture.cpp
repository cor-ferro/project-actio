#ifndef ACTIO_CORE_TEXTURE
#define ACTIO_CORE_TEXTURE

#include "texture.h"

Texture::Texture(Texture::Type type)
        : type(type)
        , name(std::string(Texture::nameByType(type))) {}

Texture::Texture(aiTextureType type)
        : type(getType(type))
        , name("") {}

Texture::Texture(Texture && otherTexture) noexcept {
    std::swap(type, otherTexture.type);
    std::swap(name, otherTexture.name);
    std::swap(renderHandle, otherTexture.renderHandle);
    std::swap(images_, otherTexture.images_);
}

Texture::Texture(const Texture &other)
        : type(other.type)
        , name(other.name)
        , images_(other.images_) // @todo: проверить удаление ресурсов. Потенциальная утечка памяти.
{}

Texture::~Texture() = default;

void Texture::destroy() {}

Texture Texture::Empty(Texture::Type type = Texture::Type::Diffuse, unsigned char color = 0) {
    auto *image = new ImageLoader::RawData[3]{color, color, color};
    std::shared_ptr<ImageData> imageData(new ImageData(image, 1, 1, GL_RGB));

    Texture texture(type);
    texture.setData(imageData);

    return texture;
}

Texture Texture::White(Texture::Type type) {
    return Texture::Empty(type, 255);
}

Texture Texture::Cube() {
    Texture texture(Texture::Type::Cube);

    return texture;
}

// @todo: осторожно добавлять данные, могут потеряться уже существующие данные
void Texture::setData(std::shared_ptr<ImageData> &imageData) {
    setData(imageData, 0);
}

void Texture::setData(std::shared_ptr<ImageData> &imageData, char index) {
    auto it = images_.find(index);
    if (it != images_.end()) {
        images_.erase(it);
    }

    images_.insert(std::pair<char, std::shared_ptr<ImageData>>(index, imageData));
}

std::shared_ptr<ImageData> Texture::getData() {
    return images_[0];
}

std::shared_ptr<ImageData> Texture::getData(char index) {
    return images_[index];
}

const Texture::TextureImages &Texture::getImages() {
    return images_;
}

Texture::Type Texture::getType()  {
    return type;
}

Texture::Type Texture::getType(aiTextureType type)  {
    switch (type) {
        case aiTextureType_DIFFUSE:
            return Texture::Type::Diffuse;
        case aiTextureType_SPECULAR:
            return Texture::Type::Specular;
        case aiTextureType_HEIGHT:
            return Texture::Type::Height;
        case aiTextureType_NORMALS:
            return Texture::Type::Normal;
        default:
            return Texture::Type::Diffuse;
    }
}

const char* Texture::charType(aiTextureType type)  {
    switch (type) {
        case aiTextureType_DIFFUSE:
            return "diffuse";
        case aiTextureType_SPECULAR:
            return "specular";
        case aiTextureType_HEIGHT:
            return "height";
        case aiTextureType_NORMALS:
            return "normal";
        default:
            return "diffuse";
    }
}

const char* Texture::nameByType(Texture::Type type)  {
    switch (type) {
        case Texture::Type::Diffuse:
            return "diffuseTexture";
        case Texture::Type::Specular:
            return "specularTexture";
        case Texture::Type::Height:
            return "heightTexture";
        case Texture::Type::Normal:
            return "normalTexture";
        case Texture::Type::Cube:
            return "cubeTexture";
        default:
            return "diffuseTexture";
    }
}

#endif
