#ifndef ACTIO_MESH_PIPELINE_H
#define ACTIO_MESH_PIPELINE_H

#include "pipeline.h"
#include "../program.h"

namespace renderer {
    namespace Opengl {
        struct MeshDrawPipeline : Pipeline {
            MeshDrawPipeline();

            ~MeshDrawPipeline() override = default;

            void draw(Mesh &mesh, uint flags) override;

            void draw(Program *program, Mesh &mesh, uint flags) override;

            void setBonesTransform(std::vector<mat4> *newBones);

        private:
            std::vector<mat4> *bones = nullptr;
        };
    }
}
#endif //ACTIO_MESH_PIPELINE_H
