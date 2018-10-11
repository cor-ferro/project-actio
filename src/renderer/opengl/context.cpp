#include "context.h"
#include "../../lib/console.h"

renderer::Opengl::Context::~Context() {
    console::info("destroy context");
    for (auto &handle : geometryHandles) {
        delete handle;
    }
    geometryHandles.clear();

    for (auto &handle : textureHandles) {
        delete handle;
    }
    textureHandles.clear();
}

renderer::Opengl::Program &renderer::Opengl::Context::getProgram(const std::string& name) {
    auto it = programs.find(name);

    assert(it != programs.end());

    return it->second;
}

renderer::Opengl::Program *renderer::Opengl::Context::findProgram(const std::string& name) {
    auto it = programs.find(name);

    if (it != programs.end()) {
        return &it->second;
    }

    return nullptr;
}

bool renderer::Opengl::Context::hasProgram(const std::string& name) {
    auto it = programs.find(name);

    return it != programs.end();
}

void renderer::Opengl::Context::createNewProgram(const std::string& name) {
    programs.emplace(name, name);
}

void renderer::Opengl::Context::addHandle(renderer::Opengl::GeometryHandle *handle) {
    geometryHandles.insert(handle);
}

void renderer::Opengl::Context::addHandle(renderer::Opengl::TextureHandle *handle) {
    textureHandles.insert(handle);
}
