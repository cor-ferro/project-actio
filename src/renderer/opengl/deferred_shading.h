//
// Created by demitriy on 10/7/18.
//

#ifndef ACTIO_RENDERER_DEFERRED_SHADING_H
#define ACTIO_RENDERER_DEFERRED_SHADING_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "utils.h"
#include "g-buffer.h"
#include "u-buffer.h"
#include "shadow-buffer.h"
#include "handle.h"
#include "shading.h"
#include "context.h"
#include "pipelines/mesh_pipeline.h"
#include "pipelines/shadow_pipeline.h"
#include "../frame_context.h"
#include "../params.h"
#include "../../core/mesh.h"
#include "../../core/geometry_builder.h"
#include "../../lib/mesh_manager.h"

namespace renderer {
    namespace Opengl {
        class DeferredShading : public Shading {
        public:
            DeferredShading() = default;

            ~DeferredShading() override = default;

            void init(Context *context, const renderer::Params& renderParams) override;

            void process(const FrameContext& frameContext) override;

        private:
            struct LightHelper {
                LightHelper() = default;

                virtual ~LightHelper() = 0;

                void init(std::shared_ptr<Mesh> &ptr);

                virtual void initMesh() = 0;

                void initHandle();

                std::shared_ptr<Mesh> mesh;
                std::shared_ptr<GeometryHandle> handle;
            };

            struct LightHelperQuad : LightHelper {
                void initMesh() override;
            };

            struct LightHelperSphere : LightHelper {
                void initMesh() override;
            };

            struct LightHelperCylinder : LightHelper {
                void initMesh() override;
            };

            void initMatricesBuffer();

            void initGBuffer();

            void initShadowBuffer();

            void initLightHelpers();

            void shadowPass(const FrameContext& frameContext);

            void geometryPass(const FrameContext& frameContext);

            void lightPass(const FrameContext& frameContext);

            void drawModels(const FrameContext& frameContext);

            void renderPointLights(const FrameContext& frameContext);

            void renderSpotLights(const FrameContext& frameContext);

            void renderDirLights(const FrameContext& frameContext);

            Dimension width = 0;
            Dimension height = 0;

            Context *context = nullptr;

            UBuffer matricesBuffer;
            GBuffer gBuffer;
            ShadowBuffer shadowBuffer;

            LightHelperQuad lightHelperQuad;
            LightHelperSphere lightHelperSphere;
            LightHelperCylinder lightHelperCylinder;

            Pipeline modelPipeline;
            Pipeline nullPipeline;
            ShadowDrawPipeline shadowPipeline;

            MeshManager meshManager;
        };
    }
}

#endif //ACTIO_RENDERER_DEFERRED_SHADING_H
