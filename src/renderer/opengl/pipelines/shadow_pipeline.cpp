#include "shadow_pipeline.h"

void renderer::Opengl::ShadowDrawPipeline::setWVP(const mat4 &mat) {
    program->setMat("gWVP", mat);
}