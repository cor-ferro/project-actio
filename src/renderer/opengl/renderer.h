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
#include "shader.h"
#include "program.h"
#include "g-buffer.h"
#include "u-buffer.h"
#include "uniforms.h"
#include "utils.h"

namespace renderer {
    struct OpenglRenderer : Renderer {
        OpenglRenderer(renderer::Params);

        ~OpenglRenderer();

        bool init();

        void draw(Scene *scene);

        void drawModels(Scene *scene, Opengl::Program &program);

        void forwardRender(Scene *scene);

        void defferedRender(Scene *scene);

        void renderPointLights(Scene &scene);

        void renderDirLights(Scene &scene);

        void renderSpotLights(Scene &scene);

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

        Mesh *lightQuad;
        Mesh *lightSphere;
        Mesh *lightCylinder;
    };

}

#endif
