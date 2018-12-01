#include "renderer.h"
#include "../lib/console.h"

namespace renderer {
    Renderer::~Renderer() = default;

    void Renderer::draw(FrameContext &frameContext) {
        switch (shading) {
            case Shading::Forward:
                forwardShading(frameContext);
                break;
            case Shading::Deferred:
                deferredShading(frameContext);
                break;
            default:
                console::warn("unknown shading type");
        }
    }

    void Renderer::enableShadows() {
        shadows = true;
    }

    void Renderer::disableShadows() {
        shadows = false;
    }

    void Renderer::setShadingType(const Shading &type) {
        shading = type;
    }

    FrameContext Renderer::createFrameContext(entityx::EntityManager &es) {
        FrameContext frameContext(es);

        frameContext.setHeight(params.getHeight());
        frameContext.setWidth(params.getWidth());
        frameContext.setShadowsEnabled(shadows);

        return frameContext;
    }

    Renderer::Renderer(const Params &params) : params(params) {

    }

    void Renderer::destroy() {

    }

    const Params &Renderer::getParams() const {
        return params;
    }

    void Renderer::registerPrograms() {
        for (auto &it : rawPrograms) {
            registerProgram(it.second);
        }
    }

    void Renderer::addProgram(const std::string &name, const RawProgram &rawProgram) {
        rawPrograms[name] = rawProgram;
    }

    void Renderer::addPrograms(const std::map<std::string, RawProgram> &newRawPrograms) {
        rawPrograms = newRawPrograms;
    }

    void Renderer::removePrograms() {
        rawPrograms.clear();
    }
}