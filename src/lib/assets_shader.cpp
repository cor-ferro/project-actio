#include "assets_shader.h"

namespace assets {
    /**
     * -------------------- Shader --------------------
     */
    Shader::Shader(assets::Resource *resource) : BaseAsset(resource) {};

    const char *const Shader::getVertexContent() const {
        return data.getRaw("vertex");
    }

    const char *const Shader::getFragmentContent() const {
        return data.getRaw("fragment");
    }

    const char *const Shader::getGeometryContent() const {
        return data.getRaw("geometry");
    }
}