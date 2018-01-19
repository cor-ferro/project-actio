#include "renderer.h"

namespace Renderer {

const float timeScale = 0.008f;
GLint maxTextureUnits;

OpenglRenderer::OpenglRenderer(RendererParams params) : BaseRenderer(params)
{

}

bool OpenglRenderer::init(int argc, char **argv)
{
	const RendererParams& renderParams = getParams();

	return true;
}

OpenglRenderer::~OpenglRenderer()
{
	console::info("destroy opengl renderer");
}

void OpenglRenderer::start()
{
	const RendererParams& renderParams = getParams();

	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
	console::info("GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS ", maxTextureUnits);

	gbuffer.init(renderParams.width, renderParams.height);

	forwardProgram.init("forward");
	forwardProgram.initUniformCache({ "model", "diffuseTexture", "heightTexture", "specularTexture", "bones[]" });
	forwardProgram.initUniformCache(Opengl::Uniform::Map);
	forwardProgram.bindBlock("Matrices", 0);
	// forwardProgram.bindBlock("Lights", 1);

	geometryPassProgram.init("geometry_pass");
	geometryPassProgram.initUniformCache({ "projection", "view", "model", "diffuseTexture", "heightTexture", "specularTexture" });
	geometryPassProgram.initUniformCache(Opengl::Uniform::Map);
	geometryPassProgram.bindBlock("Matrices", 0);
	checkGlError(__FILE__, __LINE__);

	lightPassProgram.init("light_pass");
//	lightPassProgram.bindBlock("Matrices", 0);
	// lightPassProgram.initUniformCache({ "projection", "view", "model", "diffuseTexture", "heightTexture", "specularTexture" });
	checkGlError(__FILE__, __LINE__);

	nullProgram.init("null");
	nullProgram.bindBlock("Matrices", 0);

	checkGlError(__FILE__, __LINE__);

	skyboxProgram.init("skybox");
	checkGlError(__FILE__, __LINE__);

	quad = new Mesh(Geometry::Quad2d());
	lightSphere = new Mesh(
		Geometry::Sphere(1.0f, 32, 32, 0.0f, glm::two_pi<float>(), 0.0f, 3.14f)
	);
	
	quad->setup();
	lightSphere->setup();
	checkGlError(__FILE__, __LINE__);

	// glGenFramebuffers(1, &depthMapFBO);

	// const uint SHADOW_HEIGHT = 1024;
	// const uint SHADOW_WIDTH = 1024;
	// glGenTextures(1, &depthMap);
	// glBindTexture(GL_TEXTURE_2D, depthMap);
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  
 
	// glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	// glDrawBuffer(GL_NONE);
	// glReadBuffer(GL_NONE);

	// GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	// if (status != GL_FRAMEBUFFER_COMPLETE) {
	// 	console::warn("FB error, status: 0x", status);
	// }

	// glBindFramebuffer(GL_FRAMEBUFFER, 0);

	initMatricesBuffer();
	// initLightsBuffer();
}

void OpenglRenderer::initMatricesBuffer()
{
	std::vector<size_t> sizes = {
		sizeof(mat4), // projection
		sizeof(mat4)  // view
	};

	UBufferData matricesData(1, sizes);

	std::unordered_map<std::string, UBufferData> map;
	map["main"] = matricesData;

	matricesBuffer.init(map, UBO_MATRICES_POINT_INDEX);
}

void OpenglRenderer::initLightsBuffer()
{
	const uint dirLightCount = 8;
	const uint pointLightCount = 8;
	const uint spotLightCount = 8;

	std::vector<size_t> dirLightMap = {
		sizeof(vec3), // ambient
		sizeof(vec3), // diffuse
		sizeof(vec3), // specular
		sizeof(vec3)  // direction
	};

	std::vector<size_t> pointLightMap = {
		sizeof(vec3),  // ambient
		sizeof(vec3),  // diffuse
		sizeof(vec3),  // specular
		sizeof(vec3),  // position
		sizeof(float), // constant
		sizeof(float), // linear
		sizeof(float), // quadratic
		sizeof(float)  // padding
	};

	std::vector<size_t> spotLightMap = {
		sizeof(vec3),  // ambient
		sizeof(vec3),  // diffuse
		sizeof(vec3),  // specular
		sizeof(vec3),  // position
		sizeof(vec3),  // direction
		sizeof(float), // constant
		sizeof(float), // linear
		sizeof(float), // quadratic
		sizeof(float), // cutOff
		sizeof(float)  // outerCutOff
	};

	UBufferData dirLightData(dirLightCount, dirLightMap);
	UBufferData pointLightData(1, pointLightMap);
	UBufferData spotLightData(spotLightCount, spotLightMap);

	std::unordered_map<std::string, UBufferData> map;

	// не менять местами
	// map["spot"] = spotLightData;
	map["point"] = pointLightData;
	// map["directional"] = dirLightData;

	lightBuffer.init(map, UBO_LIGHTS_POINT_INDEX);
}

void OpenglRenderer::forwardRender(Scene * scene)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Camera * camera = scene->getActiveCamera();

	const RendererParams& renderParams = getParams();

	double newTime = glfwGetTime();

	this->elaspsedTime = newTime - this->time;
	this->time = newTime;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	mat4 view = glm::translate(view, vec3(0.0f, 0.0f, -5.0f));
	mat4 projection = glm::perspective(glm::radians(45.0f), (float)renderParams.height / (float)renderParams.width, 0.1f, 100.0f);

	vec2 resolution = vec2((float)renderParams.width, (float)renderParams.height);

	if (camera != nullptr)
	{
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
		Model * skyboxModel = scene->getSkybox();
		Mesh * mesh = skyboxModel->getFirstMesh();
		Texture texture = mesh->material.getTextures().at(0);

		glActiveTexture(GL_TEXTURE0 + maxTextureUnits - 1);
		glBindTexture(texture.textureTarget, texture.textureID);
		forwardProgram.setInt("cubeTexture", maxTextureUnits - 1);
	}

	const std::vector<Light::Directional*>& dirLights = scene->getDirectionalLights();
	const std::vector<Light::Point*>& pointLights = scene->getPointLights();
	const std::vector<Light::Spot*>& spotLights = scene->getSpotLights();

	int dirLightIndex = 0;
	for (auto dirLight = dirLights.begin(); dirLight != dirLights.end(); ++dirLight)
	{
		std::string sIndex = std::to_string(dirLightIndex);
		forwardProgram.setVec("dirLights["+ sIndex +"].ambient", (*dirLight)->ambient);
		forwardProgram.setVec("dirLights["+ sIndex +"].diffuse", (*dirLight)->diffuse);
		forwardProgram.setVec("dirLights["+ sIndex +"].specular", (*dirLight)->specular);
		forwardProgram.setVec("dirLights["+ sIndex +"].direction", (*dirLight)->direction);

		dirLightIndex++;
	}

	int pointLightIndex = 0;
	for (auto pointLight = pointLights.begin(); pointLight != pointLights.end(); ++pointLight)
	{
		std::string sIndex = std::to_string(pointLightIndex);
		forwardProgram.setVec("pointLights["+ sIndex +"].ambient", (*pointLight)->ambient);
		forwardProgram.setVec("pointLights["+ sIndex +"].diffuse", (*pointLight)->diffuse);
		forwardProgram.setVec("pointLights["+ sIndex +"].specular", (*pointLight)->specular);
		forwardProgram.setVec("pointLights["+ sIndex +"].position", (*pointLight)->position);
		forwardProgram.setFloat("pointLights["+ sIndex +"].constant", (*pointLight)->constant);
		forwardProgram.setFloat("pointLights["+ sIndex +"].linear", (*pointLight)->linear);
		forwardProgram.setFloat("pointLights["+ sIndex +"].quadratic", (*pointLight)->quadratic);

		pointLightIndex++;
	}

	int spotLightIndex = 0;
	for (auto spotLight = spotLights.begin(); spotLight != spotLights.end(); ++spotLight)
	{
		std::string sIndex = std::to_string(spotLightIndex);
		forwardProgram.setVec("spotLights["+ sIndex +"].ambient", (*spotLight)->ambient);
		forwardProgram.setVec("spotLights["+ sIndex +"].diffuse", (*spotLight)->diffuse);
		forwardProgram.setVec("spotLights["+ sIndex +"].specular", (*spotLight)->specular);
		forwardProgram.setVec("spotLights["+ sIndex +"].position", (*spotLight)->position);
		forwardProgram.setVec("spotLights["+ sIndex +"].direction", (*spotLight)->direction);
		forwardProgram.setFloat("spotLights["+ sIndex +"].constant", (*spotLight)->constant);
		forwardProgram.setFloat("spotLights["+ sIndex +"].linear", (*spotLight)->linear);
		forwardProgram.setFloat("spotLights["+ sIndex +"].quadratic", (*spotLight)->quadratic);
		forwardProgram.setFloat("spotLights["+ sIndex +"].cutOff", (*spotLight)->cutOff);
		forwardProgram.setFloat("spotLights["+ sIndex +"].outerCutOff", (*spotLight)->outerCutOff);
		spotLightIndex++;
	}

	forwardProgram.setInt("countDirLights", dirLights.size());
	forwardProgram.setInt("countPointLights", pointLights.size());
	forwardProgram.setInt("countSpotLights", spotLights.size());

	// glViewport(0, 0, 1024, 1024);
	// glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	// glClear(GL_DEPTH_BUFFER_BIT);
	
	// drawModels(scene, forwardProgram);
	// glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, renderParams.width, renderParams.height);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	drawModels(scene, forwardProgram);


	// @todo: скайбокс закинуть в юниформ
	if (isHasSkybox)
	{
		glDepthFunc(GL_LEQUAL);
		skyboxProgram.use();

		Model * skyboxModel = scene->getSkybox();
		const ModelMeshes& meshes = skyboxModel->getMeshes();

		for(auto mesh = meshes.begin(); mesh != meshes.end(); mesh++)
		{
			(*mesh)->draw(skyboxProgram);
		}

		glDepthFunc(GL_LESS);
	}
}

void OpenglRenderer::defferedRender(Scene * scene)
{
	Camera * camera = scene->getActiveCamera();
	const RendererParams& renderParams = getParams();

	glViewport(0, 0, renderParams.width, renderParams.height);

	mat4 view = camera->getView();
	mat4 projection = camera->getProjection();

	matricesBuffer.use();
	matricesBuffer.set(0, glm::value_ptr(projection));
	matricesBuffer.set(1, glm::value_ptr(view));
	checkGlError(__FILE__, __LINE__);

	gbuffer.startFrame();

	// Geometry pass
		geometryPassProgram.use();
		gbuffer.geometryPass();
		checkGlError(__FILE__, __LINE__);

		glDepthMask(GL_TRUE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		checkGlError(__FILE__, __LINE__);

		drawModels(scene, geometryPassProgram);
		checkGlError(__FILE__, __LINE__, true);
		
		glDepthMask(GL_FALSE);
		checkGlError(__FILE__, __LINE__);

	// Light pass

	const std::vector<Light::Spot*>& spotLights = scene->getSpotLights();

	lightPassProgram.use();
	lightPassProgram.setInt("gPositionMap", 0);
	lightPassProgram.setInt("gNormalMap", 1);
	lightPassProgram.setInt("gAlbedoMap", 2);
	lightPassProgram.setVec("gScreenSize", vec3(1280.0f, 720.0f, 0.0f)); // fix to vec2
	lightPassProgram.setVec("gEyePosition", camera->getPosition());
	lightPassProgram.setVec("viewDir", camera->getPosition());

	glStencilFunc(GL_ALWAYS, 0, 0);
	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
	glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

	// // Point light
	gbuffer.lightPass();
	lightPassProgram.use();
	const std::vector<GLuint>& bufferTextures = gbuffer.getTextures();
	for (uint i = 0; i < bufferTextures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, bufferTextures[i]);
	}
	checkGlError(__FILE__, __LINE__);

	renderPointLights(*scene);
	renderDirLights(*scene);

	gbuffer.finalPass();

	glBlitFramebuffer(
		0, 0, renderParams.width, renderParams.height,
		0, 0, renderParams.width, renderParams.height,
		GL_COLOR_BUFFER_BIT, GL_LINEAR
	);
	checkGlError(__FILE__, __LINE__);

	glBindFramebuffer(GL_FRAMEBUFFER, DEFAULT_FRAME_BUFFER);
}

void OpenglRenderer::renderPointLights(Scene& scene)
{
	Camera * camera = scene.getActiveCamera();
	const std::vector<Light::Point*>& pointLights = scene.getPointLights();

	std::string programLightName = "PointLightType";
	lightPassProgram.setMat("projection", camera->getProjection());
	lightPassProgram.setMat("view", camera->getView());

	for (const auto pointLight : pointLights)
	{
		float lightMax  = std::fmaxf(std::fmaxf(pointLight->diffuse.r, pointLight->diffuse.g), pointLight->diffuse.b);
		float radius    = (-pointLight->linear + glm::sqrt(pointLight->linear * pointLight->linear - 4 * pointLight->quadratic * (pointLight->constant - (256.0 / 5.0) * lightMax))) / (2 * pointLight->quadratic);

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
		checkGlError(__FILE__, __LINE__);

		lightSphere->draw(nullProgram, Mesh_Draw_Base);
		checkGlError(__FILE__, __LINE__, true);

		// Light
		gbuffer.lightPass();
		lightPassProgram.use();
		glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		checkGlError(__FILE__, __LINE__);

		lightPassProgram.enableFragmentSubroutine(programLightName);
		checkGlError(__FILE__, __LINE__);
		lightPassProgram.setVec("pointLight.ambient", pointLight->ambient);
		lightPassProgram.setVec("pointLight.diffuse", pointLight->diffuse);
		lightPassProgram.setVec("pointLight.specular", pointLight->specular);
		lightPassProgram.setVec("pointLight.position", pointLight->position);
		lightPassProgram.setFloat("pointLight.constant", pointLight->constant);
		lightPassProgram.setFloat("pointLight.linear", pointLight->linear);
		lightPassProgram.setFloat("pointLight.quadratic", pointLight->quadratic);
		checkGlError(__FILE__, __LINE__);
		lightSphere->draw(lightPassProgram, Mesh_Draw_Base);
		checkGlError(__FILE__, __LINE__, true);

		glCullFace(GL_BACK);
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		checkGlError(__FILE__, __LINE__);
	}
}

void OpenglRenderer::renderDirLights(Scene& scene)
{
	const std::vector<Light::Directional*>& dirLights = scene.getDirectionalLights();
	std::string programLightName = "DirLightType";

	gbuffer.lightPass();

	lightPassProgram.use();
	lightPassProgram.setMat("projection", mat4(1.0f));
	lightPassProgram.setMat("view", mat4(1.0f));

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	for (const auto dirLight : dirLights)
	{
		lightPassProgram.setVec("dirLight.ambient", dirLight->ambient);
		lightPassProgram.setVec("dirLight.diffuse", dirLight->diffuse);
		lightPassProgram.setVec("dirLight.specular", dirLight->specular);
		lightPassProgram.setVec("dirLight.direction", dirLight->direction);
		checkGlError(__FILE__, __LINE__);
		lightPassProgram.enableFragmentSubroutine(programLightName);
		checkGlError(__FILE__, __LINE__);

		quad->draw(lightPassProgram, Mesh_Draw_Base);
		checkGlError(__FILE__, __LINE__, true);
	}

	glDisable(GL_BLEND);
}

void OpenglRenderer::drawModels(Scene * scene, Opengl::Program& program)
{
	const std::vector<Model*>& models = scene->getModels();

	for (auto modelIt = models.begin(); modelIt != models.end(); modelIt++)
	{
		Model * model = *modelIt;

		if (model->currentAnimation.isSet())
		{
			model->currentAnimation.tick(this->elaspsedTime * 1000.0);
			model->processCurrentAnimation();
		}

		const ModelMeshes& meshes = model->getMeshes();
		for(auto mesh = meshes.begin(); mesh != meshes.end(); mesh++)
		{
			(*mesh)->draw(program, Mesh_Draw_All);
	}
}
}

void OpenglRenderer::preRender()
{
	stats.startTime();
	onPreRender();
}

void OpenglRenderer::postRender()
{
	onPostRender();
	stats.updateTime();
}

void OpenglRenderer::addPreRenderHandler(callback handler)
{
	onPreRender.connect(handler);
}

void OpenglRenderer::addPostRenderHandler(callback handler)
{
	onPostRender.connect(handler);
}

void OpenglRenderer::draw(Scene * scene)
{
	preRender();

//	 forwardRender(scene);
	defferedRender(scene);

	postRender();
}

void OpenglRenderer::checkGlError(const char * file, int line, bool silent) {
	GLenum err = glGetError();

	if (silent == true) return;
	if (err == GL_NO_ERROR) return;
	
	std::string error;

	switch(err) {
		case GL_INVALID_OPERATION:      		error = "INVALID_OPERATION";      			break;
		case GL_INVALID_ENUM:           		error = "INVALID_ENUM";           			break;
		case GL_INVALID_VALUE:          		error = "INVALID_VALUE";          			break;
		case GL_OUT_OF_MEMORY:          		error = "OUT_OF_MEMORY";          			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION"; 	break;
		default: error = "unknown gl error";
	}

	if (error.size() != 0) {
		console::errp("GL: %s [%s:%i]", error.c_str(), file, line);
	}
}

}
