#include "renderer.h"

namespace Renderer {

Opengl::Program forwardProgram;
Opengl::Program edgeProgram;
Opengl::Program geometryPassProgram;
Opengl::Program skyboxProgram;

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
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
	console::info("GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS ", maxTextureUnits);

	forwardProgram.init("forward");
	forwardProgram.initUniformCache({ "model", "diffuseTexture", "heightTexture", "specularTexture", "bones[]" });
	forwardProgram.initUniformCache(Opengl::Uniform::Map);
	forwardProgram.bindBlock("Matrices", 0);
	forwardProgram.bindBlock("Lights", 1);

	geometryPassProgram.init("geometry_pass");
	geometryPassProgram.initUniformCache({ "projection", "view", "model", "diffuseTexture", "heightTexture", "specularTexture" });

	skyboxProgram.init("skybox");

	glGenFramebuffers(1, &depthMapFBO);
	
	const uint SHADOW_HEIGHT = 1024;
	const uint SHADOW_WIDTH = 1024;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
 
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		console::warn("FB error, status: 0x", status);
		// return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	initMatricesBuffer();
	initLightsBuffer();
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

	glViewport(0, 0, 1024, 1024);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	drawModels(scene);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, renderParams.width, renderParams.height);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	drawModels(scene);


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

void OpenglRenderer::drawModels(Scene * scene)
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
			(*mesh)->draw(forwardProgram);
		}
	}
}

void OpenglRenderer::defferedRender(Scene * scene)
{
	const RendererParams& renderParams = getParams();
	geometryPassProgram.use();
	gbuffer.bindForWriting();

//	geometryPassProgram.setMat("projection", projection);
//	geometryPassProgram.setMat("view", view);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	gbuffer.bindForReading();

	GLsizei HalfWidth = (GLsizei)(renderParams.width / 2.0f);
	GLsizei HalfHeight = (GLsizei)(renderParams.height / 2.0f);

	gbuffer.setReadBuffer(Renderer::GBuffer::GBUFFER_TEXTURE_TYPE_POSITION);
	glBlitFramebuffer(
		0, 0,
		renderParams.width, renderParams.height,
		0, 0,
		HalfWidth, HalfHeight,
		GL_COLOR_BUFFER_BIT, GL_LINEAR
	);

	gbuffer.setReadBuffer(Renderer::GBuffer::GBUFFER_TEXTURE_TYPE_DIFFUSE);
	glBlitFramebuffer(
		0, 0,
		renderParams.width, renderParams.height,
		0, HalfHeight,
		HalfWidth, renderParams.height,
		GL_COLOR_BUFFER_BIT, GL_LINEAR
	);

	gbuffer.setReadBuffer(Renderer::GBuffer::GBUFFER_TEXTURE_TYPE_NORMAL);
	glBlitFramebuffer(
		0, 0,
		renderParams.width, renderParams.height,
		HalfWidth, HalfHeight,
		renderParams.width, renderParams.height,
		GL_COLOR_BUFFER_BIT, GL_LINEAR
	);

	gbuffer.setReadBuffer(Renderer::GBuffer::GBUFFER_TEXTURE_TYPE_TEXCOORD);
	glBlitFramebuffer(
		0, 0,
		renderParams.width, renderParams.height,
		HalfWidth, 0,
		renderParams.width, HalfHeight,
		GL_COLOR_BUFFER_BIT, GL_LINEAR
	);
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

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// glEnable(GL_CULL_FACE);
	// glCullFace(GL_FRONT);

	forwardRender(scene);

	postRender();
}

}
