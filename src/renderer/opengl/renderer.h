#ifndef RENDERER_OPENGL_H_
#define RENDERER_OPENGL_H_

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../base_renderer.h"
#include "../../lib/console.h"
#include "../../scenes/scene.h"
#include "../../cameras/camera.h"
#include "../../lib/types.h"
#include "../../core/geometry.h"
#include "../../core/mesh.h"
#include "../../materials/material.h"
#include "shader.h"
#include "program.h"
#include "g-buffer.h"
#include "u-buffer.h"
#include "uniforms.h"
#include "utils.h"
#include "pipeline.h"

#include "../../game/components/camera.h"
#include "../../game/components/transform.h"
#include "../../game/components/target.h"
#include "../../game/components/renderable.h"
#include "../../game/components/light_directional.h"
#include "../../game/components/light_point.h"
#include "../../game/components/light_spot.h"

namespace renderer {
    using namespace game;
    namespace ex = entityx;

    struct OpenglRenderer : Renderer {
        OpenglRenderer(renderer::Params);

        ~OpenglRenderer();

        bool init();

        void draw(Scene *scene);

        void draw(entityx::EntityManager &es);

        void drawModels(Scene *scene, Opengl::Program &program);

        void drawModels(entityx::EntityManager &es);

        void forwardRender(Scene *scene);

        void deferredRender(Scene *scene);

        void deferredRender(entityx::EntityManager &es);

        void renderPointLights(ex::EntityManager &es, vec3 &cameraPosition);

        void renderSpotLights(ex::EntityManager &es, vec3 &cameraPosition);

        void renderDirLights(ex::EntityManager &es);

        void setupMesh(Mesh *mesh) override;

        void updateMesh(Mesh *mesh) override;

        void setupMaterial(Material::Phong *material);

        void destroyMesh(Mesh *mesh) override;

        GLuint depthMapFBO;
        GLuint depthMap;

        Opengl::GBuffer gbuffer;
        Opengl::UBuffer matricesBuffer;
        Opengl::UBuffer lightBuffer;
    private:
        void initMatricesBuffer();

        Opengl::Program forwardProgram;
        Opengl::Program skyboxProgram;
        Opengl::Program skyboxDeferredProgram;
        Opengl::Program geometryPassProgram;
        Opengl::Program lightPassProgram;
        Opengl::Program nullProgram;

        renderer::Opengl::MeshDrawPipeline modelPipeline;
        renderer::Opengl::MeshDrawPipeline skyboxPipeline;
        renderer::Opengl::MeshDrawPipeline nullPipeline;

        Mesh *lightQuad;
        Mesh *lightSphere;
        Mesh *lightCylinder;
    };

}

#endif
