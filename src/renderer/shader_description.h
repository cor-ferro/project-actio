//
// Created by demitriy on 7/8/18.
//

#ifndef ACTIO_RENDERER_SHADER_DESC_H
#define ACTIO_RENDERER_SHADER_DESC_H

#include "shader_content.h"

namespace renderer {
    class ShaderDescription {
    public:
        const std::string getName();

        void setName(std::string);

        void setByType(std::string type, ShaderContent content);

        void setVertexContent(ShaderContent content);

        void setFragmentContent(ShaderContent content);

        void setGeometryContent(ShaderContent content);

        const ShaderContent &getVertexContent();

        const ShaderContent &getFragmentContent();

        const ShaderContent &getGeometryContent();

    private:
        std::string name;
        ShaderContent vertexContent;
        ShaderContent fragmentContent;
        ShaderContent geometryContent;
    };
}

#endif //ACTIO_RENDERER_SHADER_DESC_H
