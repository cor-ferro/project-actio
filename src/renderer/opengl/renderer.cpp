#include "renderer.h"
#include "../../game/components/camera.h"
#include "../../game/components/skin.h"
#include "../../game/components/transform.h"
#include "../../core/geometry_primitive.h"
#include "../shader_description.h"
#include "../texture.h"

#define UBO_MATRICES_POINT_INDEX 0
#define DEFAULT_FRAME_BUFFER 0

namespace renderer {
    void MessageCallback(GLenum source,
                         GLenum type,
                         GLuint id,
                         GLenum severity,
                         GLsizei length,
                         const GLchar *message,
                         const void *userParam) {
        const char *strType = type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "";

        console::err("GL ERROR: %s type = 0x%x, severity = 0x%x, message = %s", strType, type, severity, message);
    }

    OpenglRenderer::OpenglRenderer(renderer::Params params)
            : Renderer(params)
            , skyboxDeferredProgram(nullptr)
            , geometryPassProgram(nullptr)
            , forwardProgram(nullptr)
            , skyboxProgram(nullptr)
            , lightPassProgram(nullptr)
            , nullProgram(nullptr) {}

    bool OpenglRenderer::init() {
        const renderer::Params &renderParams = getParams();

        gbuffer.init(renderParams.width, renderParams.height);

        initMatricesBuffer();
        initRequiredShaders();

        lightQuad = Mesh::Create();
        lightSphere = Mesh::Create();
        lightCylinder = Mesh::Create();

        GeometryPrimitive::Quad2d(lightQuad->geometry);
        GeometryPrimitive::Sphere(lightSphere->geometry, 1.0f, 64, 64, 0.0f, glm::two_pi<float>(), 0.0f, 3.14f);
        GeometryPrimitive::Cylinder(lightCylinder->geometry, 4.0f, 1.0f, 10.0f, 16, 16, false, 0.0f,
                                    glm::two_pi<float>());

        setupMesh(lightQuad);
        setupMesh(lightSphere);
        setupMesh(lightCylinder);

        OpenglCheckErrors();

        return true;
    }


    OpenglRenderer::~OpenglRenderer() {
        destroy();
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
////        forwardProgram.checkShadersUpdate();
//
//        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//        Camera *camera = scene->getActiveCamera();
//
//        const renderer::Params &renderParams = getParams();
//
//        glEnable(GL_DEPTH_TEST);
//        glEnable(GL_MULTISAMPLE);
//        mat4 view = glm::translate(view, vec3(0.0f, 0.0f, -5.0f));
//        mat4 projection = glm::perspective(glm::radians(45.0f),
//                                           (float) renderParams.height / (float) renderParams.width, 0.1f, 100.0f);
//
//        vec2 resolution = vec2((float) renderParams.width, (float) renderParams.height);
//
//        if (camera != nullptr) {
//            view = camera->getView();
//            projection = camera->getProjection();
//        }
//
//        matricesBuffer.use();
//        matricesBuffer.set(0, glm::value_ptr(projection));
//        matricesBuffer.set(1, glm::value_ptr(view));
//        // matricesBuffer.nouse();
//
//        forwardProgram->use();
//        forwardProgram->setFloat("time", time / 1000.0f);
//        forwardProgram->setVec("viewPos", camera->getPosition());
//
//        bool isHasSkybox = scene->hasSkybox();
//
//        if (isHasSkybox) {
//            Model *skyboxModel = scene->getSkybox();
//            Mesh *mesh = skyboxModel->getFirstMesh();
//            const std::shared_ptr<::Texture> &texture = mesh->material->getDiffuseMap();
//
//            OpenglUtils::bindTexture(GL_TEXTURE0 + maxTextureUnits - 1, *texture);
//            forwardProgram->setInt("cubeTexture", maxTextureUnits - 1);
//        }
//
//        const std::vector<Light::Directional *> &dirLights = scene->getDirectionalLights();
//        const std::vector<Light::Point *> &pointLights = scene->getPointLights();
//        const std::vector<Light::Spot *> &spotLights = scene->getSpotLights();
//
//        int dirLightIndex = 0;
//        for (auto dirLight = dirLights.begin(); dirLight != dirLights.end(); ++dirLight) {
//            std::string sIndex = std::to_string(dirLightIndex);
//            forwardProgram->setVec("dirLights[" + sIndex + "].ambient", (*dirLight)->ambient);
//            forwardProgram->setVec("dirLights[" + sIndex + "].diffuse", (*dirLight)->diffuse);
//            forwardProgram->setVec("dirLights[" + sIndex + "].specular", (*dirLight)->specular);
//            forwardProgram->setVec("dirLights[" + sIndex + "].direction", (*dirLight)->direction);
//
//            dirLightIndex++;
//        }
//
//        int pointLightIndex = 0;
//        for (auto pointLight = pointLights.begin(); pointLight != pointLights.end(); ++pointLight) {
//            std::string sIndex = std::to_string(pointLightIndex);
//            forwardProgram->setVec("pointLights[" + sIndex + "].ambient", (*pointLight)->ambient);
//            forwardProgram->setVec("pointLights[" + sIndex + "].diffuse", (*pointLight)->diffuse);
//            forwardProgram->setVec("pointLights[" + sIndex + "].specular", (*pointLight)->specular);
//            forwardProgram->setVec("pointLights[" + sIndex + "].position", (*pointLight)->position);
//            forwardProgram->setFloat("pointLights[" + sIndex + "].constant", (*pointLight)->constant);
//            forwardProgram->setFloat("pointLights[" + sIndex + "].linear", (*pointLight)->linear);
//            forwardProgram->setFloat("pointLights[" + sIndex + "].quadratic", (*pointLight)->quadratic);
//
//            pointLightIndex++;
//        }
//
//        int spotLightIndex = 0;
//        for (auto spotLight = spotLights.begin(); spotLight != spotLights.end(); ++spotLight) {
//            std::string sIndex = std::to_string(spotLightIndex);
//            forwardProgram->setVec("spotLights[" + sIndex + "].ambient", (*spotLight)->ambient);
//            forwardProgram->setVec("spotLights[" + sIndex + "].diffuse", (*spotLight)->diffuse);
//            forwardProgram->setVec("spotLights[" + sIndex + "].specular", (*spotLight)->specular);
//            forwardProgram->setVec("spotLights[" + sIndex + "].position", (*spotLight)->position);
//            forwardProgram->setVec("spotLights[" + sIndex + "].direction", (*spotLight)->direction);
//            forwardProgram->setFloat("spotLights[" + sIndex + "].constant", (*spotLight)->constant);
//            forwardProgram->setFloat("spotLights[" + sIndex + "].linear", (*spotLight)->linear);
//            forwardProgram->setFloat("spotLights[" + sIndex + "].quadratic", (*spotLight)->quadratic);
//            forwardProgram->setFloat("spotLights[" + sIndex + "].cutOff", (*spotLight)->cutOff);
//            forwardProgram->setFloat("spotLights[" + sIndex + "].outerCutOff", (*spotLight)->outerCutOff);
//            spotLightIndex++;
//        }
//
//        forwardProgram->setInt("countDirLights", dirLights.size());
//        forwardProgram->setInt("countPointLights", pointLights.size());
//        forwardProgram->setInt("countSpotLights", spotLights.size());
//
//        glViewport(0, 0, renderParams.width, renderParams.height);
//        glBindTexture(GL_TEXTURE_2D, depthMap);
//        drawModels(scene, *forwardProgram);
//
//        if (isHasSkybox) {
//            glDepthFunc(GL_LEQUAL);
//            skyboxProgram->use();
//
//            Model *skyboxModel = scene->getSkybox();
//            const ModelMeshes &meshes = skyboxModel->getMeshes();
//
//            for (auto mesh = meshes.begin(); mesh != meshes.end(); mesh++) {
////                (*mesh)->draw(*skyboxProgram);
//                console::warn("deprecated draw");
//            }
//
//            glDepthFunc(GL_LESS);
//        }
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

        lightPassProgram->use();
        lightPassProgram->setInt("gPositionMap", 0);
        lightPassProgram->setInt("gNormalMap", 1);
        lightPassProgram->setInt("gAlbedoMap", 2);
        lightPassProgram->setVec("gScreenSize", screenSize); // fix to vec2
//        lightPassProgram->setVec("viewDir", cameraPosition);

        glStencilFunc(GL_ALWAYS, 0, 0);
        glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
        glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

        // Point light
        gbuffer.lightPass();
        lightPassProgram->use();
        lightPassProgram->setMat("projection", projection);
        lightPassProgram->setMat("view", view);

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
                0, 0, gbuffer.getWidth(), gbuffer.getHeight(),
                0, 0, renderParams.width, renderParams.height,
                GL_COLOR_BUFFER_BIT, GL_LINEAR
        );
        OpenglCheckErrors();

        glBindFramebuffer(GL_FRAMEBUFFER, DEFAULT_FRAME_BUFFER);
    }

    void OpenglRenderer::renderPointLights(ex::EntityManager &es, vec3 &cameraPosition) {
//        console::info("renderPointLights");
        gbuffer.lightPass();

        ex::ComponentHandle<components::LightPoint> light;
        ex::ComponentHandle<components::Transform> transform;

        for (ex::Entity entity : es.entities_with_components(light, transform)) {
            float radius = light->getRadius();

            transform->setScale(radius);

            // Stencil
            nullProgram->use();
            gbuffer.stencilPass();

            glEnable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
            glClear(GL_STENCIL_BUFFER_BIT);

            glStencilFunc(GL_ALWAYS, 0, 0);
            glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
            glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
            OpenglCheckErrors();

            modelPipeline.setInitialTransform();
            modelPipeline.draw(nullProgram, *lightSphere, Mesh_Draw_Base);
            OpenglCheckErrorsSilent();

            // Light
            gbuffer.lightPass();
            lightPassProgram->use();

            glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_ONE, GL_ONE);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
            OpenglCheckErrors();

            lightPassProgram->setVec("gEyePosition", cameraPosition);
            lightPassProgram->setVec("pointLight.ambient", light->ambient);
            lightPassProgram->setVec("pointLight.diffuse", light->diffuse);
            lightPassProgram->setVec("pointLight.specular", light->specular);
            lightPassProgram->setVec("pointLight.position", transform->getPosition());
            lightPassProgram->setFloat("pointLight.constant", light->constant);
            lightPassProgram->setFloat("pointLight.linear", light->linear);
            lightPassProgram->setFloat("pointLight.quadratic", light->quadratic);
            lightPassProgram->enableFragmentSubroutine("getLightColor", "PointLightType");
            OpenglCheckErrors();
            modelPipeline.setTransform(*transform);
            modelPipeline.draw(lightPassProgram, *lightSphere, Mesh_Draw_Base);
            OpenglCheckErrorsSilent();

            glDisable(GL_BLEND);
            glCullFace(GL_BACK);
            glDisable(GL_CULL_FACE);
            OpenglCheckErrors();
        }
    }

    void OpenglRenderer::renderSpotLights(ex::EntityManager &es, vec3 &cameraPosition) {
//        console::info("renderSpotLights");

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
            lightPassProgram->use();
            glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_ONE, GL_ONE);
            //		glEnable(GL_CULL_FACE);
            //		glCullFace(GL_FRONT);
            OpenglCheckErrors();

            lightPassProgram->enableFragmentSubroutine("getLightColor", "SpotLightType");
            OpenglCheckErrors();
            lightPassProgram->setVec("spotLight.ambient", light->ambient);
            lightPassProgram->setVec("spotLight.diffuse", light->diffuse);
            lightPassProgram->setVec("spotLight.specular", light->specular);
            lightPassProgram->setVec("spotLight.position", light->position);
            lightPassProgram->setVec("spotLight.direction", light->direction);
            lightPassProgram->setFloat("spotLight.constant", light->constant);
            lightPassProgram->setFloat("spotLight.linear", light->linear);
            lightPassProgram->setFloat("spotLight.quadratic", light->quadratic);
            lightPassProgram->setFloat("spotLight.cutOff", light->cutOff);
            lightPassProgram->setFloat("spotLight.outerCutOff", light->outerCutOff);
            OpenglCheckErrors();
            modelPipeline.setTransform(*transform);
            modelPipeline.draw(lightPassProgram, *lightCylinder, Mesh_Draw_Base);
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

        lightPassProgram->use();
        lightPassProgram->setMat("projection", mat4(1.0f));
        lightPassProgram->setMat("view", mat4(1.0f));
        lightPassProgram->enableFragmentSubroutine("getLightColor", "DirLightType");

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_ONE, GL_ONE);

        OpenglCheckErrors();

        ex::ComponentHandle<components::LightDirectional> light;

        for (ex::Entity entity : es.entities_with_components(light)) {
            lightPassProgram->setVec("dirLight.ambient", light->ambient);
            lightPassProgram->setVec("dirLight.diffuse", light->diffuse);
            lightPassProgram->setVec("dirLight.specular", light->specular);
            lightPassProgram->setVec("dirLight.direction", light->direction);
            OpenglCheckErrors();

            modelPipeline.setInitialTransform();
            modelPipeline.draw(lightPassProgram, *lightQuad, Mesh_Draw_Base);
            OpenglCheckErrorsSilent();
        }

        glDisable(GL_BLEND);
    }

    void OpenglRenderer::drawModels(entityx::EntityManager &es) {
        modelPipeline.use();
        es.each<components::Renderable, components::Transform, components::Model>([this](
                ex::Entity entity,
                components::Renderable &,
                components::Transform &transform,
                components::Model &model
        ) {
            entityx::ComponentHandle<components::Skin> skin = entity.component<components::Skin>();

            uint renderFlags = Mesh_Draw_Base | Mesh_Draw_Textures | Mesh_Draw_Material;

            const ModelMeshes &meshes = model.getMeshes();
            for (auto &mesh : meshes) {
                if (skin) {
                    renderFlags |= Mesh_Draw_Bones;
                }

                modelPipeline.setTransform(transform); // todo: pass pointer instead of copy
                modelPipeline.draw(*mesh, renderFlags);

                stats.increaseDrawCall();
            }
        });
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

    void OpenglRenderer::destroy() {
        for (auto &handle : geometryHandles) {
            destroyGeometryHandle(handle);
        }

        geometryHandles.erase(geometryHandles.begin(), geometryHandles.end());

        for (auto &handle : textureHandles) {
            destroyTextureHandle(handle);
        }

        textureHandles.erase(textureHandles.begin(), textureHandles.end());

        if (lightQuad != nullptr) {
            Mesh::Destroy(lightQuad);
            lightQuad = nullptr;
        }

        if (lightSphere != nullptr) {
            Mesh::Destroy(lightSphere);
            lightSphere = nullptr;
        }

        if (lightCylinder != nullptr) {
            Mesh::Destroy(lightCylinder);
            lightCylinder = nullptr;
        }

        console::info("opengl renderer destroyed");
    }

    void OpenglRenderer::updateMesh(Mesh *mesh) {
        if (mesh->geometry.renderHandle == nullptr) {
            return;
        }

        const auto *handle = dynamic_cast<renderer::Opengl::GeometryHandle *>(mesh->geometry.renderHandle);

        if (handle->vbo == 0) {
            return;
        }

        GeometryVertices *vertices = mesh->geometry.getVertices();

        glBindVertexArray(handle->vao);

        glBindBuffer(GL_ARRAY_BUFFER, handle->vbo);
        if (mesh->geometry.isNeedUpdateVertices()) {
            glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(Vertex), &vertices->front(), GL_DYNAMIC_DRAW);
            mesh->geometry.setNeedUpdateVertices(false);
        } else {
            glBufferSubData(GL_ARRAY_BUFFER, 0, vertices->size() * sizeof(Vertex), &vertices->front());
        }

        glBindVertexArray(0);
        OpenglCheckErrors();
    };

    void OpenglRenderer::destroyMesh(Mesh *mesh) {
        if (mesh->geometry.renderHandle == nullptr) {
            return;
        }

        const auto *handle = dynamic_cast<renderer::Opengl::GeometryHandle *>(mesh->geometry.renderHandle);

        if (handle->vbo != 0) glDeleteBuffers(1, &handle->vbo);
        if (handle->ebo != 0) glDeleteBuffers(1, &handle->ebo);
        if (handle->vao != 0) glDeleteVertexArrays(1, &handle->vao);
    }

    void OpenglRenderer::registerShader(std::string name) {}

    void OpenglRenderer::unregisterShader(size_t id) {}

    void OpenglRenderer::regenerateBuffer() {
        const renderer::Params &renderParams = getParams();
        gbuffer.setWidth(renderParams.width);
        gbuffer.setHeight(renderParams.height);
        gbuffer.generateTextures();
    }

    void OpenglRenderer::setRequiredShaders(std::vector<ShaderDescription> list) {
        for (auto &shaderDescription : list) {
            requiredPrograms.emplace(
                    std::piecewise_construct,
                    std::forward_as_tuple(shaderDescription.getName()),
                    std::forward_as_tuple(shaderDescription)
            );
        }
    }

    void OpenglRenderer::setShaders(std::vector<ShaderDescription> list) {

    }

    void OpenglRenderer::addShaders(std::vector<ShaderDescription> list) {

    }

    void OpenglRenderer::initRequiredShaders() {
        for (auto &it : requiredPrograms) {
            Opengl::Program &program = it.second;
            program.init();
            program.bindBlock("Matrices", 0);
            program.initUniformCache();
            OpenglCheckErrors();
        }

        skyboxDeferredProgram = &requiredPrograms["skybox_deferred"];
        geometryPassProgram = &requiredPrograms["geometry_pass"];
        forwardProgram = &requiredPrograms["forward"];
        skyboxProgram = &requiredPrograms["skybox"];
        lightPassProgram = &requiredPrograms["light_pass"];
        nullProgram = &requiredPrograms["null"];

        skyboxPipeline.setProgram(skyboxDeferredProgram);
        modelPipeline.setProgram(geometryPassProgram);
        nullPipeline.setProgram(nullProgram);

        console::info("geometryPass success: %i", geometryPassProgram->isSuccess());
        console::info("lightPass success: %i", lightPassProgram->isSuccess());
        console::info("null success: %i", nullProgram->isSuccess());

        console::info("piplines inited");
    }

    void OpenglRenderer::setupMesh(Mesh *mesh) {
        if (mesh == nullptr) return;

        setupGeometry(&mesh->geometry);
        setupMaterial(mesh->material.get());
    }

    void OpenglRenderer::setupGeometry(Geometry *geometry) {
        if (geometry == nullptr) return;
        if (geometry->renderHandle != nullptr) return;

        auto *handle = createGeometryHandle();

        glGenVertexArrays(1, &handle->vao);
        glGenBuffers(1, &handle->vbo);

        glBindVertexArray(handle->vao);

        GeometryVertices *vertices = geometry->getVertices();

        switch (geometry->getType()) {
            case Geometry::Geometry_Static:
                handle->draw = GL_STATIC_DRAW;
                break;
            case Geometry::Geometry_Dynamic:
                handle->draw = GL_DYNAMIC_DRAW;
                break;
            default:
                console::warn("Unknown draw type");
                handle->draw = GL_STATIC_DRAW;
        }

        glBindBuffer(GL_ARRAY_BUFFER, handle->vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(Vertex), &vertices->front(), handle->draw);

        GeometryIndices *indices = geometry->getIndices();

        glGenBuffers(1, &handle->ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle->ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size() * sizeof(MeshIndex), &indices->front(), handle->draw);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, TexCoords));

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Tangent));

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Bitangent));

        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void *) offsetof(Vertex, BonedIDs));

        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Weights));

        glBindVertexArray(0);

        geometry->renderHandle = handle;
        handle->ready = true;
    }

    void OpenglRenderer::setupMaterial(Material *material) {
        if (material == nullptr) return;

        const std::shared_ptr<::Texture> &diffuseMap = material->getDiffuseMap();
        if (diffuseMap) {
            setupTexture(diffuseMap.get());
        }

        const std::shared_ptr<::Texture> &normalMap = material->getNormalMap();
        if (normalMap) {
            setupTexture(normalMap.get());
        }

        const std::shared_ptr<::Texture> &specularMap = material->getSpecularMap();
        if (specularMap) {
            setupTexture(specularMap.get());
        }

        const std::shared_ptr<::Texture> &heightMap = material->getHeightMap();
        if (heightMap) {
            setupTexture(specularMap.get());
        }
    }

    void OpenglRenderer::setupTexture(::Texture *texture) {
        switch (texture->getType()) {
            case ::Texture::Type::Diffuse:
            case ::Texture::Type::Specular:
            case ::Texture::Type::Height:
            case ::Texture::Type::Normal:
                setupTexture2d(texture);
                break;
            case ::Texture::Type::Cube:
                setupTextureCube(texture);
                break;
            default:
                console::warn("unknown texture type");
        }
    }

    void OpenglRenderer::setupTexture2d(::Texture *texture) {
        if (texture->renderHandle != nullptr) return;

        std::shared_ptr<ImageData> imageData = texture->getData();
        auto *handle = createTextureHandle();

        int width = imageData->getWidth();
        int height = imageData->getHeight();
        auto format = static_cast<GLenum>(imageData->getFormat());

        glGenTextures(1, &handle->textureId);
        glBindTexture(GL_TEXTURE_2D, handle->textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, imageData->get());
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        handle->target = GL_TEXTURE_2D;
        handle->ready = true;
        texture->renderHandle = handle;
    }

    void OpenglRenderer::setupTextureCube(::Texture *texture) {
        if (texture->renderHandle != nullptr) return;

        auto *handle = createTextureHandle();

        glGenTextures(1, &handle->textureId);
        glBindTexture(GL_TEXTURE_CUBE_MAP, handle->textureId);

        auto images = texture->getImages();

        int i = 0;
        for (auto &it : images) {
            std::shared_ptr<ImageData> image = it.second;
            int width = image->getWidth();
            int height = image->getHeight();
            int format = image->getFormat();

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, image.get());

            i++;
        }

        assert(i == 5);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        handle->target = GL_TEXTURE_CUBE_MAP;
        handle->ready = true;
        texture->renderHandle = handle;
    }

    void OpenglRenderer::destroyTexture(renderer::Renderer::TextureId textureId) {

    }

    Opengl::GeometryHandle *OpenglRenderer::createGeometryHandle() {
        auto *handle = new Opengl::GeometryHandle();

        geometryHandles.push_back(handle);

        return handle;
    }

    Opengl::TextureHandle *OpenglRenderer::createTextureHandle() {
        auto *handle = new Opengl::TextureHandle();

        textureHandles.push_back(handle);

        return handle;
    }

    void OpenglRenderer::destroyGeometryHandle(renderer::Opengl::GeometryHandle *handle) {
        if (handle->vbo != 0) glDeleteBuffers(1, &handle->vbo);
        if (handle->ebo != 0) glDeleteBuffers(1, &handle->ebo);
        if (handle->vao != 0) glDeleteVertexArrays(1, &handle->vao);

        handle->ready = false;
    }

    void OpenglRenderer::destroyTextureHandle(renderer::Opengl::TextureHandle *handle) {
        if (handle->textureId != 0) {
            glDeleteTextures(1, &handle->textureId);
        }

        handle->ready = false;
    }
}
