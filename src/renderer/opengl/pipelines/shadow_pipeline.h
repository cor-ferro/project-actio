#ifndef ACTIO_SHADOW_PIPELINE_H
#define ACTIO_SHADOW_PIPELINE_H

#include "pipeline.h"
#include "../program.h"
#include "../../../lib/types.h"

namespace renderer {
    namespace Opengl {
        struct ShadowDrawPipeline : Pipeline {
            ShadowDrawPipeline() = default;

            ~ShadowDrawPipeline() = default;

            void setWVP(const mat4& mat);

//            void setShadowTransforms(const std::vector<mat4>& transforms);

            void setShadowTransforms(const mat4& transform);

            void setFarPlane(const float& value);
        };
    }
}

#endif //ACTIO_SHADOW_PIPELINE_H
