#include "spot_light.h"

namespace Light {
    Spot::Spot()
            : ambient(vec3(1.0))
            , diffuse(vec3(1.0))
            , specular(vec3(1.0))
            , constant(1.0f)
            , linear(0.09f)
            , quadratic(0.032f)
            , cutOff(glm::cos(glm::radians(12.5f)))
            , outerCutOff(glm::cos(glm::radians(13.0f))) {}

    Spot::Spot(vec3 ambient, vec3 diffuse, vec3 specular)
            : ambient(ambient)
            , diffuse(diffuse)
            , specular(specular)
            , constant(1.0f)
            , linear(0.09f)
            , quadratic(0.032f)
            , cutOff(glm::cos(glm::radians(13.0f)))
            , outerCutOff(glm::cos(glm::radians(12.5f))) {}

    void Spot::setAmbient(const vec3& ambient) { this->ambient = ambient; }

    void Spot::setAmbient(float r, float g, float b) {
        ambient[0] = r;
        ambient[1] = g;
        ambient[2] = b;
    }

    void Spot::setDiffuse(const vec3& diffuse) { this->diffuse = diffuse; }

    void Spot::setDiffuse(float r, float g, float b) {
        diffuse[0] = r;
        diffuse[1] = g;
        diffuse[2] = b;
    }

    void Spot::setSpecular(const vec3& specular) { this->specular = specular; }

    void Spot::setSpecular(float r, float g, float b) {
        specular[0] = r;
        specular[1] = g;
        specular[2] = b;
    }

    const vec3& Spot::getAmbient() { return ambient; }

    const vec3& Spot::getDiffuse() { return diffuse; }

    const vec3& Spot::getSpecular() { return specular; }

    void Spot::setPosition(const vec3& position) {
        this->position = position;
    }

    void Spot::setDirection(const vec3& direction) {
        this->direction = direction;
    }

    void Spot::setAttenuation(float constant, float linear, float quadratic) {
        this->constant = constant;
        this->linear = linear;
        this->quadratic = quadratic;
    }

    void Spot::setCutoff(float cutOff, float outerCutOff) {
        this->cutOff = cutOff;
        this->outerCutOff = outerCutOff;
    }

    const vec3& Spot::getPosition() {
        return position;
    }

    const vec3& Spot::getDirection() {
        return direction;
    }
} // Light
