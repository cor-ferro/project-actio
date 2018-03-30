#include "base_renderer.h"
#include <GLFW/glfw3.h>

namespace renderer {
    Renderer::Renderer()
            : type(RenderForward) {}

    Renderer::Renderer(renderer::Params params)
            : params_(params), type(RenderForward) {}

    Renderer::~Renderer() {}

    const renderer::Params &Renderer::getParams() {
        return params_;
    }

    void Renderer::setViewSize(renderer::ScreenSize width, renderer::ScreenSize height) {
        params_.width = width;
        params_.height = height;
        params_.calcAspectRatio();
    }

    void Renderer::setType(RenderType newType) {
        type = newType;
    }

    void Renderer::setShadersFolder(Path path) {
        shadersFolder = path;
    }

    void Renderer::preRender() {
        stats.startTime();

        double newTime = glfwGetTime();

        this->elaspsedTime = newTime - this->time;
        this->time = newTime;

        onPreRender();
    }

    void Renderer::postRender() {
        onPostRender();
        stats.updateTime();
    }
}
