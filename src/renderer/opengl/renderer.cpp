#include <cassert>
#include "renderer.h"
#include "../../lib/console.h"

namespace renderer {
    OpenglRenderer::OpenglRenderer(Params &params) : Renderer(params) {

    }

    OpenglRenderer::~OpenglRenderer() {
        console::info("destruct opengl renderer");
    }

    void OpenglRenderer::init() {
        destroy();

        const renderer::Params &renderParams = getParams();

        initMatricesBuffer();
        registerPrograms();

        deferredShader.init(&context, renderParams);
        forwardShader.init(&context, renderParams);
    }

    void OpenglRenderer::initMatricesBuffer() {

    }

    void OpenglRenderer::destroy() {
        Renderer::destroy();


    }

    void OpenglRenderer::forwardShading(const FrameContext &frameContext) {
        forwardShader.process(frameContext);
    }

    void OpenglRenderer::deferredShading(const FrameContext &frameContext) {
        deferredShader.process(frameContext);
    }


    void OpenglRenderer::createMesh(std::shared_ptr<::Mesh> &mesh) {
        if (!mesh) return;

        createGeometry(&mesh->geometry);
        createMaterial(mesh->material.get());
    }

    void OpenglRenderer::updateMesh(std::shared_ptr<::Mesh> &mesh) {
        if (mesh->geometry.renderHandle == nullptr) {
            return;
        }

        const auto *handle = dynamic_cast<Opengl::GeometryHandle *>(mesh->geometry.renderHandle);

        if (handle->vbo == 0) {
            return;
        }

        GeometryVertices *vertices = mesh->geometry.getVertices();

        glBindVertexArray(handle->vao);

        glBindBuffer(GL_ARRAY_BUFFER, handle->vbo);
        if (mesh->geometry.isNeedUpdateVertices()) {
            glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(Vertex), &vertices->front(), GL_DYNAMIC_DRAW);
            mesh->geometry.setNeedUpdateVertices(false);
        } else {
            glBufferSubData(GL_ARRAY_BUFFER, 0, vertices->size() * sizeof(Vertex), &vertices->front());
        }

        glBindVertexArray(0);
        OpenglCheckErrors();
    }

    void OpenglRenderer::destroyMesh(std::shared_ptr<::Mesh> &mesh) {
        destroyGeometryHandle(dynamic_cast<Opengl::GeometryHandle*>(mesh->geometry.renderHandle));
    }


    void OpenglRenderer::createMaterial(::Material *material) {
        if (material == nullptr) return;

        const std::shared_ptr<::Texture> &diffuseMap = material->getDiffuseMap();
        if (diffuseMap) {
            createTexture(diffuseMap.get());
        }

        const std::shared_ptr<::Texture> &normalMap = material->getNormalMap();
        if (normalMap) {
            createTexture(normalMap.get());
        }

        const std::shared_ptr<::Texture> &specularMap = material->getSpecularMap();
        if (specularMap) {
            createTexture(specularMap.get());
        }

        const std::shared_ptr<::Texture> &heightMap = material->getHeightMap();
        if (heightMap) {
            createTexture(specularMap.get());
        }
    }

    void OpenglRenderer::updateMaterial(::Material *material) {

    }

    void OpenglRenderer::destroyMaterial(::Material *material) {

    }


    void OpenglRenderer::createTexture(::Texture *texture) {
        switch (texture->getType()) {
            case ::Texture::Type::Diffuse:
            case ::Texture::Type::Specular:
            case ::Texture::Type::Height:
            case ::Texture::Type::Normal:
                setupTexture2d(texture);
                break;
            case ::Texture::Type::Cube:
                setupTextureCube(texture);
                break;
            default:
                console::warn("unknown texture type");
        }
    }

    void OpenglRenderer::updateTexture(::Texture *texture) {

    }

    void OpenglRenderer::destroyTexture(::Texture *texture) {
        destroyTextureHandle(dynamic_cast<Opengl::TextureHandle*>(texture->renderHandle));
    }

    void OpenglRenderer::createGeometry(Geometry *geometry) {
        if (geometry == nullptr) return;
        if (geometry->renderHandle != nullptr) return;

        auto handle = new Opengl::GeometryHandle(geometry);

        context.addHandle(handle);
    }

    void OpenglRenderer::setupTexture2d(::Texture *texture) {
        if (texture->renderHandle != nullptr) return;

        std::shared_ptr<ImageData> imageData = texture->getData();
        auto *handle = createTextureHandle();

        int width = imageData->getWidth();
        int height = imageData->getHeight();
        auto format = static_cast<GLenum>(imageData->getFormat());

        glGenTextures(1, &handle->textureId);
        glBindTexture(GL_TEXTURE_2D, handle->textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, imageData->get());
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        handle->target = GL_TEXTURE_2D;
        handle->ready = true;
        texture->renderHandle = handle;
    }

    void OpenglRenderer::setupTextureCube(::Texture *texture) {
        if (texture->renderHandle != nullptr) return;

        auto *handle = createTextureHandle();

        glGenTextures(1, &handle->textureId);
        glBindTexture(GL_TEXTURE_CUBE_MAP, handle->textureId);

        auto images = texture->getImages();

        int i = 0;
        for (auto &it : images) {
            std::shared_ptr<ImageData> image = it.second;
            int width = image->getWidth();
            int height = image->getHeight();
            int format = image->getFormat();

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, image.get());

            i++;
        }

        assert(i == 5);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        handle->target = GL_TEXTURE_CUBE_MAP;
        handle->ready = true;
        texture->renderHandle = handle;
    }

    Opengl::TextureHandle *OpenglRenderer::createTextureHandle() {
        auto *handle = new Opengl::TextureHandle();

        context.addHandle(handle);

        return handle;
    }

    void OpenglRenderer::destroyGeometryHandle(Opengl::GeometryHandle *handle) {
        if (handle->vbo != 0) glDeleteBuffers(1, &handle->vbo);
        if (handle->ebo != 0) glDeleteBuffers(1, &handle->ebo);
        if (handle->vao != 0) glDeleteVertexArrays(1, &handle->vao);

        handle->ready = false;
    }

    void OpenglRenderer::destroyTextureHandle(Opengl::TextureHandle *handle) {
        if (handle->textureId != 0) {
            glDeleteTextures(1, &handle->textureId);
        }

        handle->ready = false;
    }

    void OpenglRenderer::registerProgram(const RawProgram &rawProgram) {
        const std::string &name = rawProgram.getName();

        assert(name.empty() && "program should has name");

        if (context.hasProgram(name)) {
            console::warn("program %s already added", name);
            return;
        }

        context.createNewProgram(name);

        auto &program = context.getProgram(name);
        program.init(rawProgram);
        program.bindBlock("Matrices", 0);
        program.initUniformCache();
        OpenglCheckErrors();
    }
}