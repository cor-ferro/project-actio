#ifndef ACTIO_OPENGL_RENDERER_NEW_H
#define ACTIO_OPENGL_RENDERER_NEW_H

#include <set>
#include "../renderer.h"
#include "../params.h"
#include "handle.h"
#include "utils.h"
#include "program.h"
#include "shading.h"
#include "context.h"
#include "forward_shading.h"
#include "deferred_shading.h"
#include "u-buffer.h"

namespace renderer {
    class OpenglRenderer : public Renderer {
    public:
        explicit OpenglRenderer(Params &params);

        ~OpenglRenderer() override;

        void init() override;

        void destroy();

        void forwardShading(const FrameContext &frameContext) override;

        void deferredShading(const FrameContext &frameContext) override;

        void registerProgram(const RawProgram &program) override;

        void createMesh(std::shared_ptr<::Mesh> &mesh) override;

        void updateMesh(std::shared_ptr<::Mesh> &mesh) override;

        void destroyMesh(std::shared_ptr<::Mesh> &mesh) override;

        void createMaterial(::Material *material) override;

        void updateMaterial(::Material *material) override;

        void destroyMaterial(::Material *material) override;

        void createTexture(::Texture *texture) override;

        void updateTexture(::Texture *texture) override;

        void destroyTexture(::Texture *texture) override;

    private:
        void initMatricesBuffer();

        void createGeometry(Geometry *geometry);

        void setupTexture2d(::Texture *texture);

        void setupTextureCube(::Texture *texture);

        Opengl::TextureHandle *createTextureHandle(); // @todo: remove

        void destroyGeometryHandle(Opengl::GeometryHandle *handle);

        void destroyTextureHandle(Opengl::TextureHandle *handle);

//        std::set<Opengl::GeometryHandle *> geometryHandles;
//        std::set<Opengl::TextureHandle *> textureHandles;
//
//        std::map<std::string, Opengl::Program> programs;

        Opengl::Context context;

        Opengl::DeferredShading deferredShader;
        Opengl::ForwardShading forwardShader;
    };
}

#endif //ACTIO_OPENGL_RENDERER_NEW_H
