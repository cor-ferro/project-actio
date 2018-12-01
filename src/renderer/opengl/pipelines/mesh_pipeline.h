#ifndef ACTIO_MESH_PIPELINE_H
#define ACTIO_MESH_PIPELINE_H

#include "pipeline.h"
#include "../program.h"

namespace renderer {
    namespace Opengl {
        struct MeshDrawPipeline : Pipeline {
            MeshDrawPipeline();

            ~MeshDrawPipeline() = default;
        };
    }
}
#endif //ACTIO_MESH_PIPELINE_H
