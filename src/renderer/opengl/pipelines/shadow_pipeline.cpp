#include "shadow_pipeline.h"

void renderer::Opengl::ShadowDrawPipeline::setWVP(const mat4 &mat) {
    program->setMat("gWVP", mat);
}

//void renderer::Opengl::ShadowDrawPipeline::setShadowTransforms(const std::vector<mat4>& transforms) {
//    program->setMat("shadowMatrices", transforms);
//}

void renderer::Opengl::ShadowDrawPipeline::setShadowTransforms(const mat4& transform) {
    program->setMat("shadowMatrices", transform);
}

void renderer::Opengl::ShadowDrawPipeline::setFarPlane(const float& value) {
    program->setFloat("farPlane", value);
}
