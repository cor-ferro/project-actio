#include "renderer.h"
#include "../../game/components/camera.h"

#define UBO_MATRICES_POINT_INDEX 0
#define DEFAULT_FRAME_BUFFER 0

namespace renderer {
    GLint maxTextureUnits;

    OpenglRenderer::OpenglRenderer(renderer::Params params)
            : Renderer(params)
            , skyboxPipeline(&skyboxDeferredProgram)
            , modelPipeline(&geometryPassProgram)
            , nullPipeline(&nullProgram) {}

    bool OpenglRenderer::init() {
        const renderer::Params &renderParams = getParams();

        gbuffer.init(renderParams.width, renderParams.height);

        forwardProgram.init("forward", shadersFolder);
        forwardProgram.initUniformCache({"model", "diffuseTexture", "heightTexture", "specularTexture", "bones[]"});
        forwardProgram.initUniformCache(Opengl::Uniform::Map);
        forwardProgram.bindBlock("Matrices", 0);

        geometryPassProgram.init("geometry_pass", shadersFolder);
        geometryPassProgram.initUniformCache(
                {"projection", "view", "model", "diffuseTexture", "heightTexture", "specularTexture", "bones[]"});
        geometryPassProgram.initUniformCache(Opengl::Uniform::Map);
        geometryPassProgram.bindBlock("Matrices", 0);
        OpenglCheckErrors();

        lightPassProgram.init("light_pass", shadersFolder);
        //	lightPassProgram.bindBlock("Matrices", 0);
        // lightPassProgram.initUniformCache({ "projection", "view", "model", "diffuseTexture", "heightTexture", "specularTexture" });
        OpenglCheckErrors();

        nullProgram.init("null", shadersFolder);
        nullProgram.bindBlock("Matrices", 0);
        OpenglCheckErrors();

        skyboxProgram.init("skybox", shadersFolder);
        skyboxProgram.bindBlock("Matrices", 0);
        OpenglCheckErrors();

        skyboxDeferredProgram.init("skybox-deferred", shadersFolder);
        skyboxDeferredProgram.bindBlock("Matrices", 0);
        OpenglCheckErrors();

        lightQuad = Mesh::Create(Geometry::Quad2d());
        lightSphere = Mesh::Create(
                Geometry::Sphere(1.0f, 32, 32, 0.0f, glm::two_pi<float>(), 0.0f, 3.14f)
        );
        lightCylinder = Mesh::Create(
                Geometry::Cylinder(
                        4.0f,
                        1.0f,
                        10.0f,
                        16,
                        16,
                        false,
                        0.0f,
                        glm::two_pi<float>()
                )
        );

        lightQuad->setup();
        lightSphere->setup();
        lightCylinder->setup();

        OpenglCheckErrors();

        initMatricesBuffer();

        return true;
    }

    OpenglRenderer::~OpenglRenderer() {
        Mesh::Destroy(lightQuad);
        Mesh::Destroy(lightSphere);
        Mesh::Destroy(lightCylinder);
        console::info("opengl renderer destroyed");
    }

    void OpenglRenderer::initMatricesBuffer() {
        std::vector<size_t> sizes = {
                sizeof(mat4), // projection
                sizeof(mat4)  // view
        };

        Opengl::UBufferData matricesData(1, sizes);

        std::unordered_map<std::string, Opengl::UBufferData> map;
        map["main"] = matricesData;

        matricesBuffer.init(map, UBO_MATRICES_POINT_INDEX);
    }

    void OpenglRenderer::forwardRender(Scene *scene) {
        forwardProgram.checkShadersUpdate();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Camera *camera = scene->getActiveCamera();

        const renderer::Params &renderParams = getParams();

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);
        mat4 view = glm::translate(view, vec3(0.0f, 0.0f, -5.0f));
        mat4 projection = glm::perspective(glm::radians(45.0f),
                                           (float) renderParams.height / (float) renderParams.width, 0.1f, 100.0f);

        vec2 resolution = vec2((float) renderParams.width, (float) renderParams.height);

        if (camera != nullptr) {
            view = camera->getView();
            projection = camera->getProjection();
        }

        matricesBuffer.use();
        matricesBuffer.set(0, glm::value_ptr(projection));
        matricesBuffer.set(1, glm::value_ptr(view));
        // matricesBuffer.nouse();

        forwardProgram.use();
        forwardProgram.setFloat("time", time / 1000.0f);
        forwardProgram.setVec("viewPos", camera->getPosition());

        bool isHasSkybox = scene->hasSkybox();

        if (isHasSkybox) {
            Model *skyboxModel = scene->getSkybox();
            Mesh *mesh = skyboxModel->getFirstMesh();
            Texture texture = mesh->material.getTextures().at(0);

            OpenglUtils::bindTexture(GL_TEXTURE0 + maxTextureUnits - 1, texture);
            forwardProgram.setInt("cubeTexture", maxTextureUnits - 1);
        }

        const std::vector<Light::Directional *> &dirLights = scene->getDirectionalLights();
        const std::vector<Light::Point *> &pointLights = scene->getPointLights();
        const std::vector<Light::Spot *> &spotLights = scene->getSpotLights();

        int dirLightIndex = 0;
        for (auto dirLight = dirLights.begin(); dirLight != dirLights.end(); ++dirLight) {
            std::string sIndex = std::to_string(dirLightIndex);
            forwardProgram.setVec("dirLights[" + sIndex + "].ambient", (*dirLight)->ambient);
            forwardProgram.setVec("dirLights[" + sIndex + "].diffuse", (*dirLight)->diffuse);
            forwardProgram.setVec("dirLights[" + sIndex + "].specular", (*dirLight)->specular);
            forwardProgram.setVec("dirLights[" + sIndex + "].direction", (*dirLight)->direction);

            dirLightIndex++;
        }

        int pointLightIndex = 0;
        for (auto pointLight = pointLights.begin(); pointLight != pointLights.end(); ++pointLight) {
            std::string sIndex = std::to_string(pointLightIndex);
            forwardProgram.setVec("pointLights[" + sIndex + "].ambient", (*pointLight)->ambient);
            forwardProgram.setVec("pointLights[" + sIndex + "].diffuse", (*pointLight)->diffuse);
            forwardProgram.setVec("pointLights[" + sIndex + "].specular", (*pointLight)->specular);
            forwardProgram.setVec("pointLights[" + sIndex + "].position", (*pointLight)->position);
            forwardProgram.setFloat("pointLights[" + sIndex + "].constant", (*pointLight)->constant);
            forwardProgram.setFloat("pointLights[" + sIndex + "].linear", (*pointLight)->linear);
            forwardProgram.setFloat("pointLights[" + sIndex + "].quadratic", (*pointLight)->quadratic);

            pointLightIndex++;
        }

        int spotLightIndex = 0;
        for (auto spotLight = spotLights.begin(); spotLight != spotLights.end(); ++spotLight) {
            std::string sIndex = std::to_string(spotLightIndex);
            forwardProgram.setVec("spotLights[" + sIndex + "].ambient", (*spotLight)->ambient);
            forwardProgram.setVec("spotLights[" + sIndex + "].diffuse", (*spotLight)->diffuse);
            forwardProgram.setVec("spotLights[" + sIndex + "].specular", (*spotLight)->specular);
            forwardProgram.setVec("spotLights[" + sIndex + "].position", (*spotLight)->position);
            forwardProgram.setVec("spotLights[" + sIndex + "].direction", (*spotLight)->direction);
            forwardProgram.setFloat("spotLights[" + sIndex + "].constant", (*spotLight)->constant);
            forwardProgram.setFloat("spotLights[" + sIndex + "].linear", (*spotLight)->linear);
            forwardProgram.setFloat("spotLights[" + sIndex + "].quadratic", (*spotLight)->quadratic);
            forwardProgram.setFloat("spotLights[" + sIndex + "].cutOff", (*spotLight)->cutOff);
            forwardProgram.setFloat("spotLights[" + sIndex + "].outerCutOff", (*spotLight)->outerCutOff);
            spotLightIndex++;
        }

        forwardProgram.setInt("countDirLights", dirLights.size());
        forwardProgram.setInt("countPointLights", pointLights.size());
        forwardProgram.setInt("countSpotLights", spotLights.size());

        glViewport(0, 0, renderParams.width, renderParams.height);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        drawModels(scene, forwardProgram);

        if (isHasSkybox) {
            glDepthFunc(GL_LEQUAL);
            skyboxProgram.use();

            Model *skyboxModel = scene->getSkybox();
            const ModelMeshes &meshes = skyboxModel->getMeshes();

            for (auto mesh = meshes.begin(); mesh != meshes.end(); mesh++) {
                (*mesh)->draw(skyboxProgram);
            }

            glDepthFunc(GL_LESS);
        }
    }

    void OpenglRenderer::deferredRender(Scene *scene) {
        console::info("todo deferred scene render");
    }

    void OpenglRenderer::deferredRender(entityx::EntityManager &es) {
        const renderer::Params &renderParams = getParams();

        glViewport(0, 0, renderParams.width, renderParams.height);

        mat4 view, projection;
        vec3 cameraPosition;

        ex::ComponentHandle<components::Camera> camera;
        ex::ComponentHandle<components::Transform> cameraTransform;
        ex::ComponentHandle<components::Target> cameraTarget;

        for (ex::Entity entity : es.entities_with_components(camera, cameraTransform, cameraTarget)) {
            view = camera->getView();
            projection = camera->getProjection();
            cameraPosition = camera->getPosition();
            break;
        }

        matricesBuffer.use();
        matricesBuffer.set(0, glm::value_ptr(projection));
        matricesBuffer.set(1, glm::value_ptr(view));
        OpenglCheckErrors();

        gbuffer.startFrame();

        // Geometry pass
        gbuffer.geometryPass();

        glDepthMask(GL_TRUE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        OpenglCheckErrors();

        drawModels(es);
        OpenglCheckErrorsSilent();

//        bool isHasSkybox = scene->hasSkybox();
//        if (isHasSkybox) {
//            Model *skyboxModel = scene->getSkybox();
//            Mesh *mesh = skyboxModel->getFirstMesh();
//            Texture texture = mesh->material.getTextures().at(0);
//
//            glDepthFunc(GL_LEQUAL);
//            OpenglUtils::bindTexture(GL_TEXTURE0 + maxTextureUnits - 1, texture);
//            skyboxDeferredProgram.use();
//            skyboxDeferredProgram.setInt("cubeTexture", maxTextureUnits - 1);
//            mesh->draw(skyboxDeferredProgram, Mesh_Draw_Base);
//            glDepthFunc(GL_LESS);
//            OpenglCheckErrorsSilent();
//        }

        glDepthMask(GL_FALSE);
        OpenglCheckErrors();

        // Light pass
        vec3 screenSize = vec3(static_cast<float>(renderParams.width), static_cast<float>(renderParams.height), 0.0f);

        lightPassProgram.use();
        lightPassProgram.setInt("gPositionMap", 0);
        lightPassProgram.setInt("gNormalMap", 1);
        lightPassProgram.setInt("gAlbedoMap", 2);
        lightPassProgram.setVec("gScreenSize", screenSize); // fix to vec2
        lightPassProgram.setVec("viewDir", cameraPosition);

        glStencilFunc(GL_ALWAYS, 0, 0);
        glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
        glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

        // Point light
        gbuffer.lightPass();
        lightPassProgram.use();
        lightPassProgram.setMat("projection", projection);
        lightPassProgram.setMat("view", view);

        const std::vector<GLuint> &bufferTextures = gbuffer.getTextures();
        for (uint i = 0; i < bufferTextures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, bufferTextures[i]);
        }
        OpenglCheckErrors();

        renderPointLights(es, cameraPosition);
        renderSpotLights(es, cameraPosition);
        renderDirLights(es);

        gbuffer.finalPass();

        glBlitFramebuffer(
                0, 0, renderParams.width, renderParams.height,
                0, 0, renderParams.width, renderParams.height,
                GL_COLOR_BUFFER_BIT, GL_LINEAR
        );
        OpenglCheckErrors();

        glBindFramebuffer(GL_FRAMEBUFFER, DEFAULT_FRAME_BUFFER);
    }

    void OpenglRenderer::renderPointLights(ex::EntityManager &es, vec3 &cameraPosition) {
//        console::info("renderPointLights");
        std::string programLightName = "PointLightType";

        ex::ComponentHandle<components::LightPoint> light;
        ex::ComponentHandle<components::Transform> transform;

        for (ex::Entity entity : es.entities_with_components(light, transform)) {
            float lightMax = std::fmaxf(
                    std::fmaxf(light->diffuse.r, light->diffuse.g),
                    light->diffuse.b
            );
            float radius = (-light->linear + glm::sqrt(light->linear * light->linear -
                                                       4 * light->quadratic *
                                                       (light->constant - (256.0f / 5.0f) * lightMax))) /
                           (2 * light->quadratic);

            lightSphere->setScale(radius);

            // Stencil
            nullProgram.use();
            gbuffer.stencilPass();

            glEnable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
            glClear(GL_STENCIL_BUFFER_BIT);

            glStencilFunc(GL_ALWAYS, 0, 0);
            glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
            glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
            OpenglCheckErrors();

            modelPipeline.draw(&nullProgram, *lightSphere, Mesh_Draw_Base);
            OpenglCheckErrorsSilent();

            // Light
            gbuffer.lightPass();
            lightPassProgram.use();
            //		glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_ONE, GL_ONE);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
            //		checkGlError(__FILE__, __LINE__);

            lightPassProgram.enableFragmentSubroutine(programLightName);
            OpenglCheckErrors();
            lightPassProgram.setVec("gEyePosition", cameraPosition);
            lightPassProgram.setVec("pointLight.ambient", light->ambient);
            lightPassProgram.setVec("pointLight.diffuse", light->diffuse);
            lightPassProgram.setVec("pointLight.specular", light->specular);
            lightPassProgram.setVec("pointLight.position", light->position);
            lightPassProgram.setFloat("pointLight.constant", light->constant);
            lightPassProgram.setFloat("pointLight.linear", light->linear);
            lightPassProgram.setFloat("pointLight.quadratic", light->quadratic);
            OpenglCheckErrors();
            modelPipeline.setObject(*transform);
            modelPipeline.draw(&lightPassProgram, *lightSphere, Mesh_Draw_Base);
            OpenglCheckErrorsSilent();

            glDisable(GL_BLEND);
            glCullFace(GL_BACK);
            glDisable(GL_CULL_FACE);
            OpenglCheckErrors();
        }
    }

    void OpenglRenderer::renderSpotLights(ex::EntityManager &es, vec3 &cameraPosition) {
//        console::info("renderSpotLights");
        std::string programLightName = "SpotLightType";

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
            gbuffer.lightPass();
            lightPassProgram.use();
            glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_ONE, GL_ONE);
            //		glEnable(GL_CULL_FACE);
            //		glCullFace(GL_FRONT);
            OpenglCheckErrors();

            lightPassProgram.enableFragmentSubroutine(programLightName);
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
            modelPipeline.setObject(*transform);
            modelPipeline.draw(&lightPassProgram, *lightCylinder, Mesh_Draw_Base);
            OpenglCheckErrorsSilent();

            glDisable(GL_BLEND);
            //		glCullFace(GL_BACK);
            //		glDisable(GL_CULL_FACE);
            OpenglCheckErrors();
        }
    }

    void OpenglRenderer::renderDirLights(ex::EntityManager &es) {
        std::string programLightName = "DirLightType";

        gbuffer.lightPass();

        lightPassProgram.use();
        lightPassProgram.setMat("projection", mat4(1.0f));
        lightPassProgram.setMat("view", mat4(1.0f));

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_ONE, GL_ONE);

        ex::ComponentHandle<components::LightDirectional> light;

        for (ex::Entity entity : es.entities_with_components(light)) {
            lightPassProgram.setVec("dirLight.ambient", light->ambient);
            lightPassProgram.setVec("dirLight.diffuse", light->diffuse);
            lightPassProgram.setVec("dirLight.specular", light->specular);
            lightPassProgram.setVec("dirLight.direction", light->direction);
            OpenglCheckErrors();
            lightPassProgram.enableFragmentSubroutine(programLightName);
            OpenglCheckErrors();

            modelPipeline.draw(&lightPassProgram, *lightQuad, Mesh_Draw_Base);
            OpenglCheckErrorsSilent();
        }

        glDisable(GL_BLEND);
    }

    void OpenglRenderer::drawModels(Scene *scene, Opengl::Program &program) {
        const std::vector<Model *> &models = scene->getModels();

        for (auto modelIt = models.begin(); modelIt != models.end(); modelIt++) {
            Model *model = *modelIt;

            uint renderFlags = Mesh_Draw_Base | Mesh_Draw_Textures | Mesh_Draw_Material;

            if (model->isAnimationProgress()) {
                renderFlags |= Mesh_Draw_Bones;
            }

            const ModelMeshes &meshes = model->getMeshes();
            for (auto mesh = meshes.begin(); mesh != meshes.end(); mesh++) {
                (*mesh)->draw(program, renderFlags);
            }
        }
    }

    void OpenglRenderer::drawModels(entityx::EntityManager &es) {
        es.each<components::Renderable, components::Transform, components::Model>([this](
                ex::Entity entity,
                components::Renderable &,
                components::Transform &transform,
                components::Model &model
        ) {
            uint renderFlags = Mesh_Draw_Base | Mesh_Draw_Textures | Mesh_Draw_Material;

            const ModelMeshes &meshes = model.getMeshes();
            for (auto &mesh : meshes) {
                modelPipeline.setObject(transform); // todo: pass pointer instead of copy
                modelPipeline.draw(*mesh, renderFlags);
            }
        });
    }

    void OpenglRenderer::draw(Scene *scene) {
        preRender();

        switch (type) {
            case RenderForward:
                forwardRender(scene);
                break;
            case RenderDeferred:
                deferredRender(scene);
                break;
        }

        postRender();
    }

    void OpenglRenderer::draw(entityx::EntityManager &es) {
        preRender();

        switch (type) {
            case RenderForward:
//                forwardRender(es);
                break;
            case RenderDeferred:
                deferredRender(es);
                break;
        }

        postRender();
    }


}
