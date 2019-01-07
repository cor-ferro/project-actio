#ifndef ACTIO_CORE_TEXTURE_H_
#define ACTIO_CORE_TEXTURE_H_

#include <glad/glad.h>

#include <memory>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include "../lib/image_data.h"
#include "../lib/image_loader.h"
#include "../lib/console.h"
#include "../resources/resources.h"
#include "../renderer/handle.h"

struct Texture {
    enum class Type {
        Diffuse = 1,
        Specular = 2,
        Height = 3,
        Normal = 4,
        Cube = 5,
        Empty = 6
    };

    typedef std::unordered_map<char, std::shared_ptr<ImageData>> TextureImages;

    explicit Texture(Texture::Type type);

    explicit Texture(aiTextureType type);

    Texture(Texture && otherTexture) noexcept;

    Texture(const Texture &texture);

    ~Texture();

    void destroy();

    static Texture Empty(Texture::Type type, unsigned char color);

    static Texture White(Texture::Type type);

    static Texture Cube();

    static Texture::Type getType(aiTextureType type);

    static const char *charType(aiTextureType type);

    static const char *nameByType(Texture::Type type);

    Texture::Type getType();

    void setData(std::shared_ptr<ImageData> &imageData);

    void setData(std::shared_ptr<ImageData> &imageData, char);

    std::shared_ptr<ImageData> getData();

    std::shared_ptr<ImageData> getData(char index);

    const TextureImages &getImages();

    Texture::Type type;
    std::string name;
    renderer::TextureHandle *renderHandle = nullptr;
private:
    TextureImages images_;
};

#endif // ACTIO_CORE_TEXTURE_H_
