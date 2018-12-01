#ifndef UNIFORMS_H_
#define UNIFORMS_H_

#include <map>

namespace renderer {
    namespace Opengl {
        namespace Uniform {
            enum Common {
                Model,
                MaterialAmbient,
                MaterialDiffuse,
                MaterialSpecular,
                MaterialShininess,
                Bones
            };

            extern std::map <Common, std::string> Map;
        }
    }
}

#endif