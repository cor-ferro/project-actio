#ifndef ACTION_CORE_MATERIAL_H_
#define ACTION_CORE_MATERIAL_H_

#include <string>
#include <vector>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include "../lib/console.h"
#include "../lib/types.h"
#include "../lib/colors.h"
#include "texture.h"
#include "../resources/resources.h"
#include "../renderer/handle.h"

class Material {
public:
    Material() = default;

    Material(const Material &otherMaterial);

    Material(Material &&otherMaterial) noexcept;

    explicit Material(color::Rgb diffuse);

    explicit Material(color::Rgb diffuse, color::Rgb ambient, color::Rgb specular);

    ~Material();

    void initEmptyTextures();

    void setAmbient(color::Rgb color);

    void setAmbient(float r, float g, float b);

    void setDiffuse(color::Rgb color);

    void setDiffuse(float r, float g, float b);

    void setSpecular(color::Rgb color);

    void setSpecular(float r, float g, float b);

    void setColor(color::Rgb color);

    void setColor(float r, float g, float b);

    void setShininess(float value);

    void setMap(Texture::Type type, std::shared_ptr<Texture> &texture);

    void setDiffuseMap(Texture texture);

    void setNormalMap(Texture texture);

    void setSpecularMap(Texture texture);

    void setHeightMap(Texture texture);

    void setDiffuseMap(std::shared_ptr<Texture> &texture);

    void setNormalMap(std::shared_ptr<Texture> &texture);

    void setSpecularMap(std::shared_ptr<Texture> &texture);

    void setHeightMap(std::shared_ptr<Texture> &texture);

    void setTilling(vec2 value);

    const vec2 &getTilling() const;

    const vec2 &getOffset() const;

    void setOffset(vec2 value);

    const std::shared_ptr<Texture> &getDiffuseMap() const;

    const std::shared_ptr<Texture> &getNormalMap() const;

    const std::shared_ptr<Texture> &getSpecularMap() const;

    const std::shared_ptr<Texture> &getHeightMap() const;

    bool isWireframe() const;

    void setWireframe(bool wireframe);

    renderer::MaterialHandle *renderHandle = nullptr;

private:
    float shininess = 0.0f;
public:
    float getShininess() const;

    const color::Rgb &getAmbient() const;

    const color::Rgb &getDiffuse() const;

    const color::Rgb &getSpecular() const;

private:
    bool wireframe = false;

    color::Rgb ambient = colors::black();
    color::Rgb diffuse = colors::black();
    color::Rgb specular = colors::black();
    std::shared_ptr<Texture> diffuseMap = nullptr;
    std::shared_ptr<Texture> normalMap = nullptr;
    std::shared_ptr<Texture> specularMap = nullptr;
    std::shared_ptr<Texture> heightMap = nullptr;

    vec2 tilling = vec2(0.0f);
    vec2 offset = vec2(0.0f);

    std::unordered_map<std::string, float> options;
};

#endif
