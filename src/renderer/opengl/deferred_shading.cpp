#include "deferred_shading.h"
#include "../../game/components/transform.h"
#include "../../game/components/renderable.h"
#include "../../game/components/meshes.h"
#include "../../game/components/model.h"
#include "../../game/components/skin.h"
#include "../../game/components/light_directional.h"
#include "../../game/components/light_point.h"
#include "../../game/components/light_spot.h"

#define UBO_MATRICES_POINT_INDEX 0
#define DEFAULT_FRAME_BUFFER 0

namespace renderer {
    namespace Opengl {
        namespace ex = entityx;
        namespace components = game::components;

        DeferredShading::LightHelper::~LightHelper() {}

        void DeferredShading::LightHelper::init() {
            initMesh();
            initHandle();
        }

        void DeferredShading::LightHelper::initMesh() {

        }

        void DeferredShading::LightHelper::initHandle() {
            handle.reset(new Opengl::GeometryHandle(&mesh->geometry));
        }

        void DeferredShading::LightHelperQuad::initMesh() {
            mesh = Mesh::Create();

            GeometryBuilder::Quad2d(mesh->geometry);
        }

        void DeferredShading::LightHelperSphere::initMesh() {
            mesh = Mesh::Create();

            GeometryBuilder::SphereDescription sphereDesc;
            sphereDesc.widthSegments = 64;
            sphereDesc.heightSegments = 64;
            GeometryBuilder::Sphere(mesh->geometry, sphereDesc);
        }

        void DeferredShading::LightHelperCylinder::initMesh() {
            mesh = Mesh::Create();

            GeometryBuilder::CylinderDescription cylinderDesc;
            cylinderDesc.radiusTop = 4.0f;
            GeometryBuilder::Cylinder(mesh->geometry, cylinderDesc);
        }

        void DeferredShading::init(Context *context, const renderer::Params& renderParams) {
            this->context = context;

            width = renderParams.getWidth();
            height = renderParams.getHeight();

            initMatricesBuffer();
            initGBuffer();
            initShadowBuffer();
            initLightHelpers();
            OpenglCheckErrors();

            modelPipeline.setProgram(context->findProgram("geometry_pass"));
            nullPipeline.setProgram(context->findProgram("null"));
            shadowPipeline.setProgram(context->findProgram("shadow_map"));

            OpenglCheckErrors();
        }

        void DeferredShading::process(const FrameContext& frameContext) {
            glViewport(0, 0, frameContext.getWidth(), frameContext.getHeight());

            matricesBuffer.use();
            matricesBuffer.set(0, glm::value_ptr(frameContext.getProjection()));
            matricesBuffer.set(1, glm::value_ptr(frameContext.getView()));
            OpenglCheckErrors();

            shadowPass(frameContext);

            gBuffer.startFrame();

            geometryPass(frameContext);
            lightPass(frameContext);

            gBuffer.finalPass();

            OpenglCheckErrors();

            glBindFramebuffer(GL_FRAMEBUFFER, DEFAULT_FRAME_BUFFER);
            OpenglCheckErrors();
        }

        void DeferredShading::shadowPass(const FrameContext& frameContext) {

        }

        void DeferredShading::geometryPass(const FrameContext& frameContext) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            // Geometry pass
            gBuffer.geometryPass();

            glDepthMask(GL_TRUE);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            OpenglCheckErrors();

            drawModels(frameContext);

            OpenglCheckErrorsSilent();

            glDepthMask(GL_FALSE);
            OpenglCheckErrors();
        }

        void DeferredShading::lightPass(const FrameContext& frameContext) {
            // Light pass
            const vec3 screenSize = vec3(
                    static_cast<float>(frameContext.getWidth()),
                    static_cast<float>(frameContext.getHeight()),
                    0.0f
            );

            Opengl::Program& lightPassProgram = context->getProgram("light_pass");

            lightPassProgram.use();
            lightPassProgram.setInt("gPositionMap", 0);
            lightPassProgram.setInt("gNormalMap", 1);
            lightPassProgram.setInt("gAlbedoMap", 2);
            lightPassProgram.setVec("gScreenSize", screenSize); // fix to vec2
//            lightPassProgram->setVec("viewDir", cameraPosition);

            glStencilFunc(GL_ALWAYS, 0, 0);
            glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
            glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

            // Point light
            gBuffer.lightPass();
            lightPassProgram.use();
            lightPassProgram.setMat("projection", frameContext.getProjection());
            lightPassProgram.setMat("view", frameContext.getView());

            const std::vector<GLuint>& bufferTextures = gBuffer.getTextures();
            for (uint i = 0; i < bufferTextures.size(); i++) {
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, bufferTextures[i]);
            }
            OpenglCheckErrors();

            renderPointLights(frameContext);
            renderSpotLights(frameContext);
            renderDirLights(frameContext);
        }

        void DeferredShading::initMatricesBuffer() {
            std::vector<size_t> sizes = {
                    sizeof(mat4), // projection
                    sizeof(mat4)  // view
            };

            Opengl::UBufferData matricesData(1, sizes);

            std::unordered_map<std::string, Opengl::UBufferData> map;
            map["main"] = matricesData;

            matricesBuffer.init(map, UBO_MATRICES_POINT_INDEX);

            OpenglCheckErrors();
        }

        void DeferredShading::initGBuffer() {
            gBuffer.init(width, height);
        }

        void DeferredShading::initShadowBuffer() {
            shadowBuffer.init(width, height);
        }

        void DeferredShading::initLightHelpers() {
            lightHelperQuad.init();
            lightHelperSphere.init();
            lightHelperCylinder.init();
        }

        void DeferredShading::drawModels(const FrameContext& frameContext) {
            modelPipeline.use();

            auto& es = frameContext.getEntityManager();

            es.each<components::Renderable, components::Transform, components::Model>([this](
                    ex::Entity entity,
                    components::Renderable&,
                    components::Transform& transform,
                    components::Model& model
            ) {
                entityx::ComponentHandle<components::Skin> skin = entity.component<components::Skin>();

                uint renderFlags = Mesh_Draw_Base | Mesh_Draw_Textures | Mesh_Draw_Material;

                const ModelMeshes& meshes = model.getMeshes();
                for (auto& mesh : meshes) {
                    if (skin) {
                        renderFlags |= Mesh_Draw_Bones;
                    }

                    modelPipeline.setTransform(transform); // todo: pass pointer instead of copy
                    modelPipeline.draw(*mesh, renderFlags);

//                    stats.increaseDrawCall();
                }
            });

            es.each<components::Renderable, components::Transform, components::Meshes>([this](
                    ex::Entity entity,
                    components::Renderable&,
                    components::Transform& transform,
                    components::Meshes& meshes
            ) {
                entityx::ComponentHandle<components::Skin> skin = entity.component<components::Skin>();

                uint renderFlags = Mesh_Draw_Base | Mesh_Draw_Textures | Mesh_Draw_Material;

                const std::vector<std::shared_ptr<Mesh>> items = meshes.items();
                for (auto& mesh : items) {
                    if (skin) {
                        renderFlags |= Mesh_Draw_Bones;
                    }

                    modelPipeline.setTransform(transform); // todo: pass pointer instead of copy
                    modelPipeline.draw(*mesh, renderFlags);

//                    stats.increaseDrawCall();
                }
            });
        }

        void DeferredShading::renderPointLights(const FrameContext& frameContext) {
//        console::info("renderPointLights");
            gBuffer.lightPass();

            auto& es = frameContext.getEntityManager();

            Opengl::Program& nullProgram = context->getProgram("null");
            Opengl::Program& lightPassProgram = context->getProgram("light_pass");

            ex::ComponentHandle<components::LightPoint> light;
            ex::ComponentHandle<components::Transform> transform;

            for (ex::Entity entity : es.entities_with_components(light, transform)) {
                float radius = light->getRadius();

                transform->setScale(radius);

                // Stencil
                nullProgram.use();
                gBuffer.stencilPass();

                glEnable(GL_DEPTH_TEST);
                glDisable(GL_CULL_FACE);
                glClear(GL_STENCIL_BUFFER_BIT);

                glStencilFunc(GL_ALWAYS, 0, 0);
                glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
                glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
                OpenglCheckErrors();

                modelPipeline.setInitialTransform();
                modelPipeline.draw(nullProgram, *lightHelperSphere.mesh.get(), Mesh_Draw_Base);
                OpenglCheckErrorsSilent();

                // Light
                gBuffer.lightPass();
                lightPassProgram.use();

                glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
                glDisable(GL_DEPTH_TEST);
                glEnable(GL_BLEND);
                glBlendEquation(GL_FUNC_ADD);
                glBlendFunc(GL_ONE, GL_ONE);
                glEnable(GL_CULL_FACE);
                glCullFace(GL_FRONT);
                OpenglCheckErrors();

                lightPassProgram.setVec("gEyePosition", frameContext.getCameraPosition());
                lightPassProgram.setVec("pointLight.ambient", light->ambient);
                lightPassProgram.setVec("pointLight.diffuse", light->diffuse);
                lightPassProgram.setVec("pointLight.specular", light->specular);
                lightPassProgram.setVec("pointLight.position", transform->getPosition());
                lightPassProgram.setFloat("pointLight.constant", light->constant);
                lightPassProgram.setFloat("pointLight.linear", light->linear);
                lightPassProgram.setFloat("pointLight.quadratic", light->quadratic);
                lightPassProgram.enableFragmentSubroutine("getLightColor", "PointLightType");
                OpenglCheckErrors();

                modelPipeline.setTransform(*transform);
                modelPipeline.draw(lightPassProgram, *lightHelperSphere.mesh.get(), Mesh_Draw_Base);
                OpenglCheckErrorsSilent();

                glDisable(GL_BLEND);
                glCullFace(GL_BACK);
                glDisable(GL_CULL_FACE);
                OpenglCheckErrors();
            }
        }

        void DeferredShading::renderSpotLights(const FrameContext& frameContext) {
//        console::info("renderSpotLights");

            auto& es = frameContext.getEntityManager();

            Opengl::Program& nullProgram = context->getProgram("null");
            Opengl::Program& lightPassProgram = context->getProgram("light_pass");

            ex::ComponentHandle<components::LightSpot> light;
            ex::ComponentHandle<components::Transform> transform;

            for (ex::Entity entity : es.entities_with_components(light, transform)) {
                // Stencil
                //		nullProgram.use();
                //		gbuffer.stencilPass();
                //
                //		glEnable(GL_DEPTH_TEST);
                //		glDisable(GL_CULL_FACE);
                //		glClear(GL_STENCIL_BUFFER_BIT);
                //
                //		glStencilFunc(GL_ALWAYS, 0, 0);
                //		glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
                //		glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
                //		checkGlError(__FILE__, __LINE__);
                //
                //		lightCylinder->draw(nullProgram, Mesh_Draw_Base);
                //		checkGlError(__FILE__, __LINE__, true);

                // Light
                gBuffer.lightPass();
                lightPassProgram.use();
                glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
                glDisable(GL_DEPTH_TEST);
                glEnable(GL_BLEND);
                glBlendEquation(GL_FUNC_ADD);
                glBlendFunc(GL_ONE, GL_ONE);
                //		glEnable(GL_CULL_FACE);
                //		glCullFace(GL_FRONT);
                OpenglCheckErrors();

                lightPassProgram.enableFragmentSubroutine("getLightColor", "SpotLightType");
                OpenglCheckErrors();
                lightPassProgram.setVec("spotLight.ambient", light->ambient);
                lightPassProgram.setVec("spotLight.diffuse", light->diffuse);
                lightPassProgram.setVec("spotLight.specular", light->specular);
                lightPassProgram.setVec("spotLight.position", light->position);
                lightPassProgram.setVec("spotLight.direction", light->direction);
                lightPassProgram.setFloat("spotLight.constant", light->constant);
                lightPassProgram.setFloat("spotLight.linear", light->linear);
                lightPassProgram.setFloat("spotLight.quadratic", light->quadratic);
                lightPassProgram.setFloat("spotLight.cutOff", light->cutOff);
                lightPassProgram.setFloat("spotLight.outerCutOff", light->outerCutOff);
                OpenglCheckErrors();
                modelPipeline.setTransform(*transform);
                modelPipeline.draw(lightPassProgram, *lightHelperCylinder.mesh.get(), Mesh_Draw_Base);
                OpenglCheckErrorsSilent();

                glDisable(GL_BLEND);
                //		glCullFace(GL_BACK);
                //		glDisable(GL_CULL_FACE);
                OpenglCheckErrors();
            }
        }

        void DeferredShading::renderDirLights(const FrameContext& frameContext) {
            std::string programLightName = "DirLightType";

            gBuffer.lightPass();

            Opengl::Program& lightPassProgram = context->getProgram("light_pass");

            lightPassProgram.use();
            lightPassProgram.setMat("projection", mat4(1.0f));
            lightPassProgram.setMat("view", mat4(1.0f));
            lightPassProgram.enableFragmentSubroutine("getLightColor", "DirLightType");

            glDisable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_ONE, GL_ONE);

            OpenglCheckErrors();

            auto& es = frameContext.getEntityManager();

            ex::ComponentHandle<components::LightDirectional> light;

            for (ex::Entity entity : es.entities_with_components(light)) {
                lightPassProgram.setVec("dirLight.ambient", light->ambient);
                lightPassProgram.setVec("dirLight.diffuse", light->diffuse);
                lightPassProgram.setVec("dirLight.specular", light->specular);
                lightPassProgram.setVec("dirLight.direction", light->direction);
                OpenglCheckErrors();

                modelPipeline.setInitialTransform();
                modelPipeline.draw(lightPassProgram, *lightHelperQuad.mesh.get(), Mesh_Draw_Base);
                OpenglCheckErrorsSilent();
            }

            glDisable(GL_BLEND);
        }
    }
}