#include "material.h"
#include <memory>

Material::Material(const Material &otherMaterial) {
    shininess = otherMaterial.shininess;
    wireframe = otherMaterial.wireframe;
    ambient = otherMaterial.ambient;
    diffuse = otherMaterial.diffuse;
    specular = otherMaterial.specular;
    diffuseMap = otherMaterial.diffuseMap;
    normalMap = otherMaterial.normalMap;
    specularMap = otherMaterial.specularMap;
    heightMap = otherMaterial.heightMap;
    tilling = otherMaterial.tilling;
    offset = otherMaterial.offset;
}

Material::Material(Material &&otherMaterial) noexcept {
    std::swap(shininess, otherMaterial.shininess);
    std::swap(wireframe, otherMaterial.wireframe);
    std::swap(ambient, otherMaterial.ambient);
    std::swap(diffuse, otherMaterial.diffuse);
    std::swap(specular, otherMaterial.specular);
    std::swap(diffuseMap, otherMaterial.diffuseMap);
    std::swap(normalMap, otherMaterial.normalMap);
    std::swap(specularMap, otherMaterial.specularMap);
    std::swap(heightMap, otherMaterial.heightMap);
    std::swap(tilling, otherMaterial.tilling);
    std::swap(offset, otherMaterial.offset);
}

Material::Material(color::Rgb diffuse)
        : diffuse(diffuse) {}

Material::Material(color::Rgb diffuse, color::Rgb ambient, color::Rgb specular)
        : diffuse(diffuse)
        , ambient(ambient)
        , specular(specular) {}

Material::~Material() {
    console::info("destroy material");
}

void Material::initEmptyTextures() {
    std::shared_ptr<Texture> diffuseMap = std::make_shared<Texture>(Texture::White(Texture::Type::Diffuse));
    std::shared_ptr<Texture> normalMap = std::make_shared<Texture>(Texture::White(Texture::Type::Normal));
    std::shared_ptr<Texture> specularMap = std::make_shared<Texture>(Texture::Empty(Texture::Type::Specular, 1));
    std::shared_ptr<Texture> heightMap = std::make_shared<Texture>(Texture::White(Texture::Type::Height));

    setDiffuseMap(diffuseMap);
    setNormalMap(normalMap);
    setSpecularMap(specularMap);
    setHeightMap(heightMap);
}

void Material::setDiffuse(const float r, const float g, const float b) {
    setDiffuse(color::Rgb(r, g, b));
}

void Material::setDiffuse(const color::Rgb color) {
    diffuse = color;
}

void Material::setAmbient(const float r, const float g, const float b) {
    setAmbient(color::Rgb(r, g, b));
}

void Material::setAmbient(const color::Rgb color) {
    ambient = color;
}

void Material::setSpecular(const float r, const float g, const float b) {
    setSpecular(color::Rgb(r, g, b));
};

void Material::setSpecular(color::Rgb color) {
    specular = color;
}

void Material::setColor(const float r, const float g, const float b) {
    setDiffuse(r, g, b);
    setAmbient(r, g, b);
    setSpecular(r, g, b);
}

void Material::setColor(const color::Rgb color) {
    setDiffuse(color);
    setAmbient(color);
    setSpecular(color);
}

void Material::setShininess(const float value) {
    shininess = value;
}

void Material::setMap(Texture::Type type, std::shared_ptr<Texture> &texture) {
    switch (type) {
        case Texture::Type::Diffuse:
            setDiffuseMap(texture);
            break;
        case Texture::Type::Normal:
            setNormalMap(texture);
            break;
        case Texture::Type::Specular:
            setSpecularMap(texture);
            break;
        case Texture::Type::Height:
            setHeightMap(texture);
            break;
        default:
            console::warn("set unknown material map type");
    }
}

void Material::setDiffuseMap(Texture texture) {
    diffuseMap = std::make_shared<Texture>(texture);
};

void Material::setNormalMap(Texture texture) {
    normalMap = std::make_shared<Texture>(texture);
};

void Material::setSpecularMap(Texture texture) {
    specularMap = std::make_shared<Texture>(texture);
};

void Material::setHeightMap(Texture texture) {
    heightMap = std::make_shared<Texture>(texture);
};

void Material::setDiffuseMap(std::shared_ptr<Texture> &texture) {
    diffuseMap = texture;
}

void Material::setNormalMap(std::shared_ptr<Texture> &texture) {
    normalMap = texture;
}

void Material::setSpecularMap(std::shared_ptr<Texture> &texture) {
    specularMap = texture;
}

void Material::setHeightMap(std::shared_ptr<Texture> &texture) {
    heightMap = texture;
}

void Material::setTilling(vec2 value) {
    tilling = value;
}

void Material::setOffset(vec2 value) {
    offset = value;
}

const vec2 &Material::getTilling() const {
    return tilling;
}

const vec2 &Material::getOffset() const {
    return offset;
}

const std::shared_ptr<Texture> &Material::getDiffuseMap() const {
    return diffuseMap;
}

const std::shared_ptr<Texture> &Material::getNormalMap() const {
    return normalMap;
}

const std::shared_ptr<Texture> &Material::getSpecularMap() const {
    return specularMap;
}

const std::shared_ptr<Texture> &Material::getHeightMap() const {
    return heightMap;
}

bool Material::isWireframe() const {
    return wireframe;
}

void Material::setWireframe(bool wireframe) {
    Material::wireframe = wireframe;
}

float Material::getShininess() const {
    return shininess;
}

const color::Rgb &Material::getAmbient() const {
    return ambient;
}

const color::Rgb &Material::getDiffuse() const {
    return diffuse;
}

const color::Rgb &Material::getSpecular() const {
    return specular;
}
