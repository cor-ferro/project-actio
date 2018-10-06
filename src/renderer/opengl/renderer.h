#ifndef RENDERER_OPENGL_H_
#define RENDERER_OPENGL_H_

#include <vector>
#include <memory>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../base_renderer.h"
#include "../shader_content.h"
#include "../../lib/console.h"
#include "../../cameras/camera.h"
#include "../../lib/types.h"
#include "../../core/geometry.h"
#include "../../core/mesh.h"
#include "../../core/material.h"
#include "shader.h"
#include "program.h"
#include "g-buffer.h"
#include "shadow-buffer.h"
#include "u-buffer.h"
#include "uniforms.h"
#include "utils.h"
#include "pipelines/mesh_pipeline.h"
#include "pipelines/shadow_pipeline.h"

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

        struct FrameContext {
            explicit FrameContext(const renderer::Params &renderParams, entityx::EntityManager &es) : es(es) {
                setScreenHeight(renderParams.height);
                setScreenWidth(renderParams.width);

                ex::ComponentHandle<components::Camera> camera;
                ex::ComponentHandle<components::Transform> cameraTransform;
                ex::ComponentHandle<components::Target> cameraTarget;

                for (ex::Entity entity : es.entities_with_components(camera, cameraTransform, cameraTarget)) {
                    setView(camera->getView());
                    setProjection(camera->getProjection());
                    setCameraPosition(camera->getPosition());
                    setCameraRotation(camera->getRotation());
                    setCameraRotationMat(camera->getRotationMat());
                    break;
                }
            };

            mat4 view;
            mat4 projection;
            vec3 cameraPosition;
            vec3 cameraRotation;
            mat4 cameraRotationMat;
            entityx::EntityManager &es;
            renderer::ScreenSize screenHeight = 0;
            renderer::ScreenSize screenWidth = 0;

            const renderer::ScreenSize &getScreenWidth() const {
                return screenWidth;
            }

            void setScreenWidth(ScreenSize screenWidth) {
                FrameContext::screenWidth = screenWidth;
            }

            const renderer::ScreenSize &getScreenHeight() const {
                return screenHeight;
            }

            void setScreenHeight(renderer::ScreenSize screenHeight) {
                FrameContext::screenHeight = screenHeight;
            }

            void setView(const mat4 &value) { view = value; }

            void setProjection(const mat4 &value) { projection = value; };

            void setCameraPosition(const vec3 &value) { cameraPosition = value; }

            void setCameraRotation(const vec3 &value) { cameraRotation = value; }

            void setCameraRotationMat(const mat4 &value) { cameraRotationMat = value; };

            const mat4 &getView() const { return view; }

            const mat4 &getProjection() const { return projection; }

            const vec3 &getCameraPosition() const { return cameraPosition; }

            const vec3 &getCameraRotation() const { return cameraRotation; }

            const mat4 &getCameraRotationMat() const { return cameraRotationMat; }

            entityx::EntityManager &getEntityManager() const { return es; };
        };

        explicit OpenglRenderer(renderer::Params);

        ~OpenglRenderer() override;

        bool init() override;

        void draw(entityx::EntityManager &es) override;

        void destroy() override;

        void destroyGeometryHandle(renderer::Opengl::GeometryHandle *handle);

        void destroyTextureHandle(renderer::Opengl::TextureHandle *handle);

        void drawModels(const FrameContext &frameContext);

        void deferredRender(const FrameContext &frameContext);

        void renderPointLights(const FrameContext &frameContext);

        void renderSpotLights(const FrameContext &frameContext);

        void renderDirLights(const FrameContext &frameContext);

        void updateMesh(std::shared_ptr<Mesh>) override;

        void destroyMesh(std::shared_ptr<Mesh>) override;

        void registerShader(std::string name) override;

        void unregisterShader(size_t id) override;

        void regenerateBuffer() override;

        void setRequiredShaders(std::vector<ShaderDescription> list) override;

        void setShaders(std::vector<ShaderDescription> list) override;

        void addShaders(std::vector<ShaderDescription> list) override;

        void setupMesh(std::shared_ptr<Mesh>) override;

        void setupGeometry(Geometry *geometry) override;

        void setupMaterial(Material *material) override;

        void setupTexture(::Texture *texture) override;

        void setupTexture2d(::Texture *texture);

        void setupTextureCube(::Texture *texture);

        void destroyTexture(TextureId textureId) override;

        GLuint depthMapFBO;
        GLuint depthMap;

        Opengl::GBuffer gbuffer;
        Opengl::ShadowBuffer shadowBuffer;
        Opengl::UBuffer matricesBuffer;
        Opengl::UBuffer lightBuffer;
    private:
        void initMatricesBuffer();

        void initRequiredShaders();

        void shadowPass(const FrameContext &frameContext);

        void geometryPass(const FrameContext &frameContext);

        void lightPass(const FrameContext &frameContext);

        Opengl::GeometryHandle *createGeometryHandle();

        Opengl::TextureHandle *createTextureHandle();

        Opengl::Program *forwardProgram;
        Opengl::Program *skyboxProgram;
        Opengl::Program *skyboxDeferredProgram;
        Opengl::Program *geometryPassProgram;
        Opengl::Program *lightPassProgram;
        Opengl::Program *nullProgram;
        Opengl::Program *shadowProgram;
        Opengl::Program *shadowMapProgram;

        std::unordered_map<size_t, Opengl::Program> programs;

        renderer::Opengl::MeshDrawPipeline modelPipeline;
        renderer::Opengl::MeshDrawPipeline skyboxPipeline;
        renderer::Opengl::MeshDrawPipeline nullPipeline;
        renderer::Opengl::ShadowDrawPipeline shadowPipeline;

        std::shared_ptr<Mesh> lightQuad = nullptr;
        std::shared_ptr<Mesh> lightSphere = nullptr;
        std::shared_ptr<Mesh> lightCylinder = nullptr;

        std::unordered_map<std::string, Opengl::Program> requiredPrograms;
        std::unordered_map<std::string, Opengl::Program> optionalPrograms;

        std::vector<Opengl::GeometryHandle *> geometryHandles;
        std::vector<Opengl::TextureHandle *> textureHandles;
    };

}

#endif
