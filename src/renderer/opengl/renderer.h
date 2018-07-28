#ifndef RENDERER_OPENGL_H_
#define RENDERER_OPENGL_H_

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../base_renderer.h"
#include "../shader_content.h"
#include "../../lib/console.h"
#include "../../scenes/scene.h"
#include "../../cameras/camera.h"
#include "../../lib/types.h"
#include "../../core/geometry.h"
#include "../../core/mesh.h"
#include "../../core/material.h"
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

#include "handle.h"

namespace renderer {
    using namespace game;
    namespace ex = entityx;

    static void GLAPIENTRY MessageCallback(GLenum source,
                                           GLenum type,
                                           GLuint id,
                                           GLenum severity,
                                           GLsizei length,
                                           const GLchar *message,
                                           const void *userParam);

    struct OpenglRenderer : Renderer {
        struct RenderGeometry {
            enum GeometryType {
                Geometry_Static = 1,
                Geometry_Dynamic = 2,
            };

            GLuint VAO = 0;
            GLuint VBO = 0;
            GLuint EBO = 0;

            GeometryType type = Geometry_Static;
        };

        struct RenderTexture {
            GLuint id;
            GLenum target;
        };

        OpenglRenderer(renderer::Params);

        ~OpenglRenderer();

        bool init();

        void draw(Scene *scene);

        void draw(entityx::EntityManager &es);

        void destroy();

        void destroyGeometryHandle(renderer::Opengl::GeometryHandle *handle);

        void destroyTextureHandle(renderer::Opengl::TextureHandle *handle);

        void drawModels(entityx::EntityManager &es);

        void forwardRender(Scene *scene);

        void deferredRender(Scene *scene);

        void deferredRender(entityx::EntityManager &es);

        void renderPointLights(ex::EntityManager &es, vec3 &cameraPosition);

        void renderSpotLights(ex::EntityManager &es, vec3 &cameraPosition);

        void renderDirLights(ex::EntityManager &es);

        void updateMesh(Mesh *mesh) override;

        void destroyMesh(Mesh *mesh) override;

        void registerShader(std::string name) override;

        void unregisterShader(size_t id) override;

        void regenerateBuffer() override;

        void setRequiredShaders(std::vector<ShaderDescription> list) override;

        void setShaders(std::vector<ShaderDescription> list) override;

        void addShaders(std::vector<ShaderDescription> list) override;

        void setupMesh(Mesh *mesh) override;

        void setupGeometry(Geometry *geometry) override;

        void setupMaterial(Material *material) override;

        void setupTexture(::Texture *texture) override;

        void setupTexture2d(::Texture *texture);

        void setupTextureCube(::Texture *texture);

        void destroyTexture(TextureId textureId) override;

        GLuint depthMapFBO;
        GLuint depthMap;

        Opengl::GBuffer gbuffer;
        Opengl::UBuffer matricesBuffer;
        Opengl::UBuffer lightBuffer;
    private:
        void initMatricesBuffer();

        void initRequiredShaders();

        Opengl::GeometryHandle* createGeometryHandle();

        Opengl::TextureHandle* createTextureHandle();

        Opengl::Program *forwardProgram;
        Opengl::Program *skyboxProgram;
        Opengl::Program *skyboxDeferredProgram;
        Opengl::Program *geometryPassProgram;
        Opengl::Program *lightPassProgram;
        Opengl::Program *nullProgram;

        std::unordered_map<size_t, Opengl::Program> programs;

        renderer::Opengl::MeshDrawPipeline modelPipeline;
        renderer::Opengl::MeshDrawPipeline skyboxPipeline;
        renderer::Opengl::MeshDrawPipeline nullPipeline;

        Mesh *lightQuad = nullptr;
        Mesh *lightSphere = nullptr;
        Mesh *lightCylinder = nullptr;

        std::unordered_map<std::string, Opengl::Program> requiredPrograms;
        std::unordered_map<std::string, Opengl::Program> optionalPrograms;

        std::vector<Opengl::GeometryHandle*> geometryHandles;
        std::vector<Opengl::TextureHandle*> textureHandles;
    };

}

#endif
