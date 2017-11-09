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
	timerId = ++globalTimerId;
}

bool OpenglRenderer::init(int argc, char **argv)
{
	const RendererParams& renderParams = getParams();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_MULTISAMPLE);
	glutInitWindowPosition(50, 25);
	glutInitWindowSize(renderParams.width, renderParams.height);

	winId = glutCreateWindow("");

	if (glewInit() != GLEW_OK)
	{
		console::err("failed init glew");
		return false;
	}

	glViewport(0, 0, renderParams.width, renderParams.height);

	glutDisplayFunc(drawFunction);
//	glutTimerFunc(SECOND / FPS, timerFunction, timerId);
	glutReshapeFunc(resizeFunction);

	return true;
}

OpenglRenderer::~OpenglRenderer()
{
	console::info("destroy opengl renderer");
}

void OpenglRenderer::start()
{
	currentRenderer = this;
	console::info("renderer main loop");
	Scene * scene = getScene();
	if (!scene) {
		console::warn("Scene pointer is empty. Cannot start render.");
		return;
	}

	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
	console::info("GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS", maxTextureUnits);

	forwardProgram.init("forward");
	forwardProgram.initUniformCache({ "model", "diffuseTexture", "heightTexture", "specularTexture" });
	forwardProgram.bindBlock("Matrices", 0);
//	forwardProgram.bindBlock("Lights", 1);

	geometryPassProgram.init("geometry_pass");
	geometryPassProgram.initUniformCache({ "projection", "view", "model", "diffuseTexture", "heightTexture", "specularTexture" });

	skyboxProgram.init("skybox");

	initMatricesBuffer();
//	initLightsBuffer();
}

void OpenglRenderer::setTitle(const char * text)
{
	glutSetWindowTitle(text);
}

void OpenglRenderer::initMatricesBuffer()
{
	// projection, view
	matricesBuffer.init({ sizeof(mat4), sizeof(mat4) }, UBO_MATRICES_POINT_INDEX);
}

void OpenglRenderer::initLightsBuffer()
{
	auto dataSize = sizeof(float);

	glGenBuffers(1, &uboLights);
	glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
	glBufferData(GL_UNIFORM_BUFFER, dataSize, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(GL_UNIFORM_BUFFER, UBO_LIGHTS_POINT_INDEX, uboLights, 0, dataSize);
}

void OpenglRenderer::forwardRender()
{
	Scene * scene = getScene();
	Camera * camera = scene->getActiveCamera();

	const RendererParams& renderParams = getParams();
	GLfloat time = glutGet(GLUT_ELAPSED_TIME) * timeScale;

	glEnable(GL_DEPTH_TEST);
	// glDepthMask(GL_TRUE);
	glEnable(GL_MULTISAMPLE);
	glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
	glOrtho(-renderParams.aspectRatio, renderParams.aspectRatio, -1, 1, -1, 1);

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
	matricesBuffer.nouse();

	forwardProgram.use();
	forwardProgram.setFloat("time", time);
	forwardProgram.setVec("viewPos", camera->getPosition());

	bool isHasSkybox = scene->hasSkybox();

	if (isHasSkybox) {
		Model * skyboxModel = scene->getSkybox();
		Mesh * mesh = skyboxModel->getMeshes()->at(0);
		Texture texture = mesh->material.getTextures().at(0);

		glActiveTexture(GL_TEXTURE0 + maxTextureUnits - 1);
		glBindTexture(texture.textureTarget, texture.textureID);
		forwardProgram.setInt("cubeTexture", maxTextureUnits - 1);
	}

	std::vector<Renderer::Light::Directional*> * dirLights = scene->getDirectionalLights();
	std::vector<Renderer::Light::Point*> * pointLights = scene->getPointLights();
	std::vector<Renderer::Light::Spot*> * spotLights = scene->getSpotLights();

	int dirLightIndex = 0;
	for (auto dirLight = dirLights->begin(); dirLight != dirLights->end(); ++dirLight)
	{
		std::string sIndex = std::to_string(dirLightIndex);
		forwardProgram.setVec("dirLights["+ sIndex +"].ambient", (*dirLight)->ambient);
		forwardProgram.setVec("dirLights["+ sIndex +"].diffuse", (*dirLight)->diffuse);
		forwardProgram.setVec("dirLights["+ sIndex +"].specular", (*dirLight)->specular);
		forwardProgram.setVec("dirLights["+ sIndex +"].direction", (*dirLight)->direction);
		dirLightIndex++;
	}

	int pointLightIndex = 0;
	for (auto pointLight = pointLights->begin(); pointLight != pointLights->end(); ++pointLight)
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
	for (auto spotLight = spotLights->begin(); spotLight != spotLights->end(); ++spotLight)
	{
		std::string sIndex = std::to_string(spotLightIndex);
		forwardProgram.setVec("spotLights["+ sIndex +"].ambient", (*spotLight)->ambient);
		forwardProgram.setVec("spotLights["+ sIndex +"].diffuse", (*spotLight)->diffuse);
		forwardProgram.setVec("spotLights["+ sIndex +"].specular", (*spotLight)->specular);
		forwardProgram.setVec("spotLights["+ sIndex +"].position", (*spotLight)->position);
		forwardProgram.setVec("spotLights["+ sIndex +"].direction", (*spotLight)->direction);
		forwardProgram.setFloat("spotLights["+ sIndex +"].constant", (*spotLight)->constant);
		forwardProgram.setFloat("spotLights["+ sIndex +"].linear", (*spotLight)->linear);
		forwardProgram.setFloat("spotLights["+ sIndex +"0].quadratic", (*spotLight)->quadratic);
		forwardProgram.setFloat("spotLights["+ sIndex +"].cutOff", (*spotLight)->cutOff);
		forwardProgram.setFloat("spotLights["+ sIndex +"].outerCutOff", (*spotLight)->outerCutOff);
		spotLightIndex++;
	}

	forwardProgram.setInt("countDirLights", dirLights->size());
	forwardProgram.setInt("countPointLights", pointLights->size());
	forwardProgram.setInt("countSpotLights", spotLights->size());

	std::vector<Model*> * models = scene->getModels();
	for (auto model = models->begin(); model != models->end(); model++)
	{
		std::vector<Mesh*> * meshes = (*model)->getMeshes();
		for(auto mesh = meshes->begin(); mesh != meshes->end(); mesh++)
		{
			(*mesh)->draw(forwardProgram);
		}
	}

	if (isHasSkybox)
	{
		glDepthFunc(GL_LEQUAL);
		skyboxProgram.use();
		skyboxProgram.setMat("projection", projection);
		skyboxProgram.setMat("view", glm::mat4(glm::mat3(view)));

		Model * skyboxModel = scene->getSkybox();
		std::vector<Mesh*> * meshes = skyboxModel->getMeshes();

		for(auto mesh = meshes->begin(); mesh != meshes->end(); mesh++)
		{
			(*mesh)->draw(skyboxProgram);
		}

		glDepthFunc(GL_LESS);
	}
}

void OpenglRenderer::defferedRender()
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
	preFrameSignal_();
}

void OpenglRenderer::postRender()
{
	postFrameSignal_();
	stats.updateTime();
}

void OpenglRenderer::addPreRenderHandler(callback handler)
{
	preFrameSignal_.connect(handler);
}

void OpenglRenderer::addPostRenderHandler(callback handler)
{
	postFrameSignal_.connect(handler);
}

void OpenglRenderer::draw()
{
	if (InputHandler::instance().isPress(KEY_ESC))
	{
		glutDestroyWindow(winId);
		return;
	}

	OpenglRenderer * renderer = currentRenderer;

	preRender();

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);

	forwardRender();

	glEnd();
	glFlush();
	glutSwapBuffers();

	postRender();
}

void resizeFunction(int width, int height) {
	console::info("resize function", width, height);

	OpenglRenderer * renderer = currentRenderer;

	renderer->setViewSize(width, height);
	glViewport(0,0,width,height);
	InputHandler::instance().setWinSize(width, height);
}

void drawFunction() {};

}
