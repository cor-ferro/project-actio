#ifndef ACTIO_RENDERER_NEW_H
#define ACTIO_RENDERER_NEW_H

#include <memory>
#include <entityx/Entity.h>
#include "frame_context.h"
#include "raw_program.h"
#include "../lib/types.h"
#include "../core/mesh.h"
#include "../core/material.h"
#include "../core/texture.h"

namespace renderer {
    enum class Shading {
        Forward,
        Deferred
    };

    class Renderer {
    private:
        bool shadows = false;
        Shading shading = Shading::Forward;
        Params params;

    public:
        explicit Renderer(const Params &params);

        virtual ~Renderer() = 0;

        virtual void init() = 0;

        void destroy();

        void draw(FrameContext &frameContext);

        void setShadingType(const Shading &type);

        void enableShadows();

        void disableShadows();

        const Params &getParams() const;

        virtual void forwardShading(const FrameContext &frameContext) = 0;

        virtual void deferredShading(const FrameContext &frameContext) = 0;

        void addProgram(const std::string &name, const RawProgram &rawProgram);

        void addPrograms(const std::map<std::string, RawProgram> &rawPrograms);

        void removePrograms();

        void registerPrograms();

        virtual void registerProgram(const RawProgram &program) = 0;

        virtual void createMesh(std::shared_ptr<::Mesh> &mesh) = 0;

        virtual void updateMesh(std::shared_ptr<::Mesh> &mesh) = 0;

        virtual void destroyMesh(std::shared_ptr<::Mesh> &mesh) = 0;

        virtual void createMaterial(::Material *material) = 0;

        virtual void updateMaterial(::Material *material) = 0;

        virtual void destroyMaterial(::Material *material) = 0;

        virtual void createTexture(::Texture *texture) = 0;

        virtual void updateTexture(::Texture *texture) = 0;

        virtual void destroyTexture(::Texture *texture) = 0;

        FrameContext createFrameContext(entityx::EntityManager &es);

    protected:
        std::map<std::string, RawProgram> rawPrograms;

    };
}

#endif //ACTIO_RENDERER_H
