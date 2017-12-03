#include "uniforms.h"

namespace Opengl {
    namespace Uniform {
        std::map <Common, std::string> Map = { 
            { Model, "model" }, 
            { MaterialAmbient, "material.ambient" },
            { MaterialDiffuse, "material.diffuse" },
            { MaterialSpecular, "material.specular" },
            { MaterialShininess, "material.shininess" },
            { Bones, "bones[]" },
        };
    }
}