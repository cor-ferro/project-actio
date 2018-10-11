#ifndef ACTIO_RENDERER_RAW_PROGRAM_H
#define ACTIO_RENDERER_RAW_PROGRAM_H

#include <map>
#include <vector>
#include <tuple>
#include "shader_content.h"

namespace renderer {
    class RawProgram {
    public:
        enum ShaderType {
            VertexShader,
            FragmentShader,
            GeometryShader
        };

        RawProgram() = default;
        explicit RawProgram(const std::string &name);

        ShaderContent &getShader(const std::string &type);
        const ShaderContent &getShader(const std::string &type) const;
        const ShaderContent &getVertexShader() const;
        const ShaderContent &getFragmentShader() const;
        const ShaderContent &getGeometryShader() const;

        const std::string &getName() const;

        void setName(const std::string &name);

        static bool isValidType(const std::string &type);

    private:
        std::string name;

        ShaderContent vertexShader;
        ShaderContent fragmentShader;
        ShaderContent geometryShader;
    };
}

#endif //ACTIO_RENDERER_RAW_PROGRAM_H
