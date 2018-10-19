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
            cylinderDesc.radiusBottom = 0.0f;
            cylinderDesc.height = 30.0f;
            cylinderDesc.radialSegments = 16;
            cylinderDesc.heightSegments = 16;
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
            if (frameContext.isShadowsEnabled() && shadowBuffer.isCompleted()) {
                shadowBuffer.bindForWriting();
                shadowPipeline.use();

                glDrawBuffer(GL_NONE);
                glDepthMask(GL_TRUE);
                glEnable(GL_DEPTH_TEST);

                glClear(GL_DEPTH_BUFFER_BIT);

                auto& es = frameContext.getEntityManager();

                float aspect = static_cast<float>(frameContext.width) / static_cast<float>(frameContext.height);
                float near = 1.0f;
                float far = 65.0f;
                mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far);

                shadowPipeline.setFarPlane(far);
                OpenglCheckErrors();

                ex::ComponentHandle<components::LightPoint> light;
                ex::ComponentHandle<components::Transform> lightTransform;

                for (ex::Entity lightEntity : es.entities_with_components(light, lightTransform)) {
                    const vec3& lightPos = lightTransform->getPosition();

//                    std::vector<mat4> shadowTransforms;
//                    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + vec3( 0.0,-1.0, 0.0), vec3(0.0, 0.0,-1.0)));
//                    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + vec3( 0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0)));
//                    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + vec3( 0.0, 0.0, 1.0), vec3(0.0,-1.0, 0.0)));
//                    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + vec3(-1.0, 0.0, 0.0), vec3(0.0,-1.0, 0.0)));
//                    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + vec3( 1.0, 0.0, 0.0), vec3(0.0,-1.0, 0.0)));
//                    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + vec3( 0.0, 0.0,-1.0), vec3(0.0,-1.0, 0.0)));


//                    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f) ));
//                    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + vec3(-1.0f, 0.0f, 0.0f),vec3(0.0f, -1.0f, 0.0f) ));
//                    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f) ));
//                    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + vec3(0.0f, -1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f) ));
//                    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f) ));
//                    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, -1.0f, 0.0f)));


                    OpenglCheckErrors();

                    ex::ComponentHandle<components::Renderable> objectRenderable;
                    ex::ComponentHandle<components::Transform> objectTransform;
                    ex::ComponentHandle<components::Meshes> objectMeshes;

                    shadowPipeline.setShadowTransforms(shadowProj * glm::lookAt(lightPos, lightPos + vec3(0.0,0.0,1.0), vec3(0.0, 0.0,1.0)));
                    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

                    for (ex::Entity objectEntity : es.entities_with_components(objectRenderable, objectTransform, objectMeshes)) {
                        const std::vector<std::shared_ptr<Mesh>> items = objectMeshes->items();
                        for (auto &mesh : items) {
                            shadowPipeline.setTransform(*objectTransform); // todo: pass pointer instead of copy
                            OpenglCheckErrors();
                            shadowPipeline.draw(*mesh, Mesh_Draw_Base);
                            OpenglCheckErrors();
                        }
                    }


                }




//                const mat4 &cameraRotateMat = frameContext.getCameraRotationMat();
//
//                ex::ComponentHandle<components::LightPoint> light;
//                ex::ComponentHandle<components::Transform> lightTransform;
//
//                for (ex::Entity lightEntity : es.entities_with_components(light, lightTransform)) {
//                    // try move from loop
//                    ex::ComponentHandle<components::Renderable> objectRenderable;
//                    ex::ComponentHandle<components::Transform> objectTransform;
//                    ex::ComponentHandle<components::Meshes> objectMeshes;
//
//                    const mat4 proj = frameContext.getProjection();
//                    const mat4 transM = glm::translate(mat4(1.0f), lightTransform->getPosition() * -1.0f);
//                    const mat4 lightCameraView = cameraRotateMat * transM; // for 3d person reverse operands
//                    const mat4 m_WorldTransformation = mat4(1.0f);
//                    const mat4 gWVP = proj * lightCameraView * m_WorldTransformation;
//
//                    for (ex::Entity objectEntity : es.entities_with_components(objectRenderable, objectTransform, objectMeshes)) {
//                        shadowPipeline.setWVP(gWVP);
//
//                        const std::vector<std::shared_ptr<Mesh>> items = objectMeshes->items();
//                        for (auto &mesh : items) {
//                            shadowPipeline.setTransform(*objectTransform); // todo: pass pointer instead of copy
//                            shadowPipeline.draw(*mesh, Mesh_Draw_Base);
//                        }
//                    }
//                }
            }
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
            shadowBuffer.init(800, 800);
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

                if (frameContext.isShadowsEnabled()) {
                    glActiveTexture(GL_TEXTURE10);
                    glBindTexture(GL_TEXTURE_CUBE_MAP, shadowBuffer.getShadowMap());
                    lightPassProgram.setInt("depthMap", 10);
                    OpenglCheckErrors();
                }

                lightPassProgram.setFloat("farPlane", radius);
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
//                		nullProgram.use();
//                		gBuffer.stencilPass();
//
//                		glEnable(GL_DEPTH_TEST);
//                		glDisable(GL_CULL_FACE);
//                		glClear(GL_STENCIL_BUFFER_BIT);
//
//                		glStencilFunc(GL_ALWAYS, 0, 0);
//                		glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
//                		glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
//                		OpenglCheckErrors();
//
//                        modelPipeline.setTransform(*transform);
//                        modelPipeline.draw(lightPassProgram, *lightHelperCylinder.mesh.get(), Mesh_Draw_Base);
//                		OpenglCheckErrorsSilent();

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
//                		glCullFace(GL_BACK);
//                		glDisable(GL_CULL_FACE);
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