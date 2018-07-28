#include "material.h"

namespace renderer {
    void Material::setShader(std::shared_ptr<Shader> &newShader) {
        shader = newShader;
    }

    void Material::setDiffuseMap(std::shared_ptr<Texture> &texture) {
        diffuseMap = texture;
    };

    void Material::setHeightMap(std::shared_ptr<Texture> &texture) {
        heightMap = texture;
    };

    void Material::setNormalMap(std::shared_ptr<Texture> &texture) {
        normalMap = texture;
    };

    void Material::setAmbientColor(color::Rgb color) {
        ambient = color;
    };

    void Material::setAmbientColor(color::Val r, color::Val g, color::Val b) {
        setAmbientColor(color::Rgb(r, g, b));
    };

    void Material::setDiffuseColor(color::Rgb color) {
        diffuse = color;
    };

    void Material::setDiffuseColor(color::Val r, color::Val g, color::Val b) {
        setDiffuseColor(color::Rgb(r, g, b));
    };

    void Material::setSpecularColor(color::Rgb color) {
        specular = color;
    };

    void Material::setSpecularColor(color::Val r, color::Val g, color::Val b) {
        setSpecularColor(color::Rgb(r, g, b));
    };

    void Material::setShininess(color::Val value) {
        shininess = value;
    };
}