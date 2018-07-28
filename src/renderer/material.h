//
// Created by demitriy on 7/21/18.
//

#ifndef ACTIO_RENDERER_MATERIAL_H
#define ACTIO_RENDERER_MATERIAL_H

#include <memory>
#include "texture.h"
#include "shader.h"
#include "../lib/types.h"

namespace renderer {
    class Material {
    public:
        Material() = default;

        void setShader(std::shared_ptr<Shader> &newShader);

        void setDiffuseMap(std::shared_ptr<Texture> &texture);

        void setHeightMap(std::shared_ptr<Texture> &texture);

        void setNormalMap(std::shared_ptr<Texture> &texture);

        void setAmbientColor(color::Rgb color);

        void setAmbientColor(color::Val r, color::Val g, color::Val b);

        void setDiffuseColor(color::Rgb color);

        void setDiffuseColor(color::Val r, color::Val g, color::Val b);

        void setSpecularColor(color::Rgb color);

        void setSpecularColor(color::Val r, color::Val g, color::Val b);

        void setShininess(color::Val value);

    private:
        std::shared_ptr<Shader> shader;

        std::shared_ptr<Texture> diffuseMap;
        std::shared_ptr<Texture> heightMap;
        std::shared_ptr<Texture> normalMap;

        color::Rgb ambient;
        color::Rgb diffuse;
        color::Rgb specular;

        float shininess = 0.0f;
    };
}

#endif //ACTIO_RENDERER_MATERIAL_H
