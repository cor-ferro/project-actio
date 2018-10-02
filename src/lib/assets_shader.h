//
// Created by demitriy on 9/30/18.
//

#ifndef ACTIO_ASSETS_SHADER_H
#define ACTIO_ASSETS_SHADER_H

#include "assets_base.h"
#include "assets_resource.h"
#include "text_set.h"

namespace assets {
    class Shader : public BaseAsset {
    public:
        explicit Shader(assets::Resource *resource);

        ~Shader() = default;

        const char *const getVertexContent() const;

        const char *const getFragmentContent() const;

        const char *const getGeometryContent() const;

    private:
        TextSet data;
    };
}

#endif //ACTIO_ASSETS_SHADER_H
