#define NDEBUG

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// #include <memory>
#include <signal.h>
#include <boost/bind.hpp>

#include "PxPhysics.h"
#include "PxScene.h"
#include "PxRigidDynamic.h"
#include "PxShape.h"
#include "PxPhysicsAPI.h"
#include "pvd/PxPvd.h"

#include "glm/gtc/random.hpp"

#include "app/app.h"
#include "ag.h"

#include "renderer/window_context.h"
#include "lib/types.h"
#include "lib/cycle.h"
#include "lib/input_handler.h"
#include "lib/console.h"
#include "lib/utils.h"
#include "renderer/renderer.h"
#include "scenes/scene.h"
#include "cameras/camera.h"
#include "cameras/camera_control.h"
#include "lights/direction_light.h"
#include "core/model.h"
#include "helpers/arrow_helper.h"
#include "helpers/bounding_box_helper.h"
#include "helpers/point_light_helper.h"
#include "helpers/camera_orientation_helper.h"
#include "resources/resources.h"
#include "memory/freelistallocator.h"
#include "memory/linearallocator.h"
#include "memory/poolallocator.h"

using namespace std;
using namespace physx;

PxDefaultAllocator gAllocator;
PxDefaultErrorCallback gErrorCallback;

PxFoundation* gFoundation = NULL;
PxPhysics*  gPhysics = NULL;
PxPvd* gPvd = NULL;
PxDefaultCpuDispatcher* gDispatcher = NULL;
PxScene* gScene = NULL;
PxMaterial* gMaterial = NULL;

void initPhysics();
void stepPhysics();
void cleanupPhysics();
void cleanupScene(Scene * scene);

PxRigidDynamic* createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity=PxVec3(0));

memory::FreeListAllocator * imageAllocator = nullptr;
memory::PoolAllocator * modelsAllocator = nullptr;
memory::PoolAllocator * meshAllocator = nullptr;

int main(int argc, char **argv) {
	App& app = App::instance();
	app.setName("project actio");
	app.init(argc, argv);

	console::info("start ", app.getName());
	console::info("");
	console::infop("sizeof Model: %i", sizeof(Model));
	console::infop("sizeof Mesh: %i", sizeof(Mesh));
	console::info("");

	imageAllocator = new memory::FreeListAllocator(1e8, memory::FreeListAllocator::FIND_FIRST);
	imageAllocator->Init();

	modelsAllocator = new memory::PoolAllocator(sizeof(Model) * 10000, sizeof(Model));
	modelsAllocator->Init();

	meshAllocator = new memory::PoolAllocator(sizeof(Mesh) * 50000, sizeof(Mesh));
	meshAllocator->Init();

	unsigned int windowWidth = 1280;
	unsigned int windowHeight = 720;

	WindowContext mainContext;
	
	if (!mainContext.init(windowWidth, windowHeight)) {
		return -1;
	}

	mainContext.setTitle(app.getName().c_str());

	RendererParams rendererParams;
	rendererParams.width = windowWidth;
	rendererParams.height = windowHeight;
	rendererParams.calcAspectRatio();

	// init renderer before scene loading
	std::shared_ptr<Renderer::OpenglRenderer> renderer(new Renderer::OpenglRenderer(rendererParams));
	renderer->init(argc, argv);
	renderer->start();

	initPhysics();

	InputHandler inputHandler(mainContext);

	std::shared_ptr<Scene> scene(new Scene());
	console::info("init scene");
	Resource::File testScene("testScene.ini");
	scene->initFromFile(testScene);
	console::info("-------memory-------");
	console::info("images: ", utils::formatMemorySize(imageAllocator->getUsed()));
	console::info("models: ", utils::formatMemorySize(modelsAllocator->getUsed()));
	console::info("mesh: ", utils::formatMemorySize(meshAllocator->getUsed()));
	console::info("--------------------");

//	std::shared_ptr<Light::Spot> spotLight(AG::Light::spot());
//	spotLight->setAmbient(vec3(0.1f));
//	spotLight->setDiffuse(vec3(0.1f));
//	spotLight->setSpecular(vec3(5.15f));
//	spotLight->setPosition(vec3(5.0f, 0.0f, 5.0f));
//	spotLight->setDirection(vec3(0.0f, -1.0f, 0.0f));
//	spotLight->setAttenuation(1.0f, 0.1f, 0.012f);
//	spotLight->setCutoff(glm::cos(glm::radians(13.0f)), glm::cos(glm::radians(12.0f)));

	Camera * sceneActiveCamera = scene->getActiveCamera();
	CameraControl cameraControl(sceneActiveCamera, &inputHandler);
	sceneActiveCamera->setParam(CameraParam::ASPECT, rendererParams.aspectRatio);

	Material::Phong material;
	material.setDiffuse(0.0f, 1.0f, 0.0f);
	Geometry geometry = Geometry::Torus(5.0f, 1.0f, 16, 100, glm::two_pi<float>());

	for (int i = 0; i < 0; i++) {
		Mesh * mesh = Mesh::Create(geometry, material);
		mesh->setPosition(vec3(
			glm::gaussRand(0.0f, 7.0f),
			glm::gaussRand(0.0f, 7.0f),
			glm::gaussRand(0.0f, 7.0f)
		));
		Model * model = Model::Create(mesh);

		scene->add(model);
	}

	Light::Point * pointLight = scene->getPointLight(0);
	if (pointLight != nullptr) {
		Helpers::PointLight * pointLightHelper = new Helpers::PointLight(pointLight);

		scene->add(pointLightHelper);
		renderer->onPreRender.connect(boost::bind(&Helpers::PointLight::beforeRender, pointLightHelper));
	}

	Model * mainModel = scene->getModelByName("Megabeth");

	Model * planeModel = AG::Models::plane(10, 10, 10, 10);
	planeModel->rotate(vec3(0.0f, 1.0f, 1.0f), glm::pi<float>());
	scene->add(planeModel);

	Model * sphereModel = AG::Models::sphere(3.0f, 16, 16);
	scene->add(sphereModel);

	PxRigidDynamic* ball = createDynamic(PxTransform(PxVec3(0, 20, 0)), PxSphereGeometry(3), PxVec3(0, -25, -5));
	PxRigidBodyExt::updateMassAndInertia(*ball, 1000.0f);

	GLFWwindow * const window = mainContext.getWindow();

	mainContext.enableVSync();

	while(!glfwWindowShouldClose(window))
	{
		float time = (float)glfwGetTime();
		cameraControl.update();

		if (pointLight != nullptr) {
			vec3 centerPoint(0.0, 10.0f, 0.0f);
			float rad = (glm::sin(time * 0.1f)) * glm::two_pi<float>();
			quat newQuat = glm::angleAxis(rad, vec3(1.0f, 1.0f, 1.0f));
			vec3 newLightPos = centerPoint * newQuat;

			pointLight->setPosition(newLightPos);
		}

		inputHandler.onFrame();

		stepPhysics();

		if (!ball->isSleeping()) {
			PxTransform ballTransform = ball->getGlobalPose();

			sphereModel->setPosition(ballTransform.p.x, ballTransform.p.y, ballTransform.p.z);
			sphereModel->setQuaternion(ballTransform.q.x, ballTransform.q.y, ballTransform.q.z, ballTransform.q.w);
		}

		if (mainModel != nullptr) {
			mainModel->rotate(vec3(0.0f, 0.0f, 1.0f), 0.03f);
		}

		renderer->draw(scene.get());

//		ImGui_ImplGlfwGL3_NewFrame();
//		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), true);
//
//		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoInputs|ImGuiWindowFlags_NoBringToFrontOnFocus;
//
//		ImGui::Begin("Metrics", NULL, flags);
//		ImGui::Text("%.3f ms, %.1f fps", renderer->stats.msFrame, renderer->stats.fps);
//		ImGui::SetWindowSize(ImVec2(200.0f, 30.0f));
//		ImGui::End();
//		ImGui::Render();

		glfwSwapBuffers(window);
		glfwPollEvents();

		if (inputHandler.isPress(InputHandler::KEY_ESC))
		{
			mainContext.destroy();
			break;
		}
	}

	cleanupPhysics();
	cleanupScene(scene.get());

	std::vector<Model*> sceneModels = scene->getModels();

	for (uint i = 0; i < sceneModels.size(); i++) {
		Model * model = sceneModels[i];
		Model::Destroy(model);
	}

	console::info("after loop free");

//	sceneModels.clear();

//	for (auto model : models) {
//		Model::Destroy(model);
//		console::info("destroy success");
//		model = nullptr;
//	}

	console::info("-------memory-------");
	console::info("images: ", utils::formatMemorySize(imageAllocator->getUsed()));
	console::info("models: ", utils::formatMemorySize(modelsAllocator->getUsed()));
	console::info("mesh: ", utils::formatMemorySize(meshAllocator->getUsed()));
	console::info("--------------------");

	delete imageAllocator;
	delete modelsAllocator;
	delete meshAllocator;

	console::info("stop application");

	return 0;
}

void initPhysics()
{
	gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gAllocator, gErrorCallback);
	gPvd = PxCreatePvd(*gFoundation);
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);
	gDispatcher = PxDefaultCpuDispatcherCreate(4);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;

	sceneDesc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;
	sceneDesc.flags |= PxSceneFlag::eENABLE_PCM;
	sceneDesc.flags |= PxSceneFlag::eENABLE_STABILIZATION;
	sceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;
	sceneDesc.gpuMaxNumPartitions = 8;

	gScene = gPhysics->createScene(sceneDesc);

	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
	PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *gMaterial);
	gScene->addActor(*groundPlane);

	console::info("physics inited");
}

void stepPhysics()
{
	PX_UNUSED(false);
	gScene->simulate(1.0f/60.0f);
	gScene->fetchResults(true);
}

void cleanupPhysics()
{
	if (gScene) {
		gScene->release();
		gScene = NULL;
	}

	if (gDispatcher) {
		gDispatcher->release();
		gDispatcher = NULL;
	}

	if (gPhysics) {
		gPhysics->release();
		gPhysics = NULL;
	}

	if (gFoundation) {
		gFoundation->release();
		gFoundation = NULL;
	}

	console::info("physics released");
}

void cleanupScene(Scene * scene)
{

}

PxRigidDynamic * createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity)
{
	PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, t, geometry, *gMaterial, 10.0f);
	dynamic->setAngularDamping(0.5f);
	dynamic->setLinearVelocity(velocity);
	gScene->addActor(*dynamic);

	return dynamic;
}
