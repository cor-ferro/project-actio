#include "shader_description.h"
#include "../lib/console.h"

namespace renderer {
    const std::string ShaderDescription::getName() {
        return name;
    }

    void ShaderDescription::setName(const std::string newName) {
        name = newName;
    }

    void ShaderDescription::setByType(std::string type, renderer::ShaderContent content) {
        if (type == "vert") {
            setVertexContent(content);
        } else if (type == "frag") {
            setFragmentContent(content);
        } else if (type == "geom") {
            setGeometryContent(content);
        } else {
            console::warn("invalid shader description type: %s", type);
        }
    }

    void ShaderDescription::setVertexContent(renderer::ShaderContent content) {
        vertexContent = content;
    }

    void ShaderDescription::setFragmentContent(renderer::ShaderContent content) {
        fragmentContent = content;
    }

    void ShaderDescription::setGeometryContent(renderer::ShaderContent content) {
        geometryContent = content;
    }

    const ShaderContent &ShaderDescription::getVertexContent() {
        return vertexContent;
    };

    const ShaderContent &ShaderDescription::getFragmentContent() {
        return fragmentContent;
    };

    const ShaderContent &ShaderDescription::getGeometryContent() {
        return geometryContent;
    };
}