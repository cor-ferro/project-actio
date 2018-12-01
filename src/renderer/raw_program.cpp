#include "raw_program.h"

renderer::RawProgram::RawProgram(const std::string &name) : name(name) {

}

renderer::ShaderContent &renderer::RawProgram::getShader(const std::string &type) {
    if (type == "vert" || type == "vertex") {
        return vertexShader;
    } else if (type == "frag" || type == "fragment") {
        return fragmentShader;
    } else if (type == "geom" || type == "geometry") {
        return geometryShader;
    } else {
        throw "invalid shader type";
    }
}

const renderer::ShaderContent &renderer::RawProgram::getShader(const std::string &type) const {
    if (type == "vert" && type == "vertex") {
        return getVertexShader();
    } else if (type == "frag" && type == "fragment") {
        return getFragmentShader();
    } else if (type == "geom" && type == "geometry") {
        return getGeometryShader();
    } else {
        throw "invalid shader type";
    }
}

const renderer::ShaderContent &renderer::RawProgram::getVertexShader() const {
    return vertexShader;
}

const renderer::ShaderContent &renderer::RawProgram::getFragmentShader() const {
    return fragmentShader;
}

const renderer::ShaderContent &renderer::RawProgram::getGeometryShader() const {
    return geometryShader;
}

const std::string &renderer::RawProgram::getName() const {
    return name;
}

void renderer::RawProgram::setName(const std::string &name) {
    RawProgram::name = name;
}

bool renderer::RawProgram::isValidType(const std::string& type) {
    if (type == "vert" || type == "vertex") {
        return true;
    } else if (type == "frag" || type == "fragment") {
        return true;
    } else if (type == "geom" || type == "geometry") {
        return true;
    }

    return false;
};
