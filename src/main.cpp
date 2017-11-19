// #include <memory>
#include <signal.h>
#include "./app/app.h"

#include "ag.h"

#include "./lib/types.h"
#include "./lib/cycle.h"
#include "./lib/input_handler.h"
#include "./lib/console.h"
#include "./lib/scoped_ptr.h"
#include "./renderer/renderer.h"
#include "./scenes/scene.h"
#include "./cameras/camera.h"
#include "./cameras/camera_control.h"
#include "./lights/direction_light.h"
#include "./core/model.h"
#include "./helpers/arrow_helper.h"
#include "./helpers/camera_orientation_helper.h"
#include "./resources/resources.h"

using namespace std;

void posix_death_signal(int signum)
{
	console::warn("app fall");
	signal(signum, SIG_DFL);
	exit(3);
}

int main(int argc, char **argv) {
	// signal(SIGSEGV, posix_death_signal);

	App& app = App::instance();
	app.setName("project actio");
	app.init(argc, argv);

	console::info("start", app.getName());

	RendererParams rendererParams;
	rendererParams.width = 1280;
	rendererParams.height = 720;
	rendererParams.calcAspectRatio();

	// init renderer before scene loading

	std::shared_ptr<Renderer::OpenglRenderer>
	renderer(new Renderer::OpenglRenderer(rendererParams));
	renderer->init(argc, argv);
	renderer->setTitle(app.getName().c_str());

	InputHandler &inputHandler = InputHandler::instance();

	std::shared_ptr<Scene> scene(AG::createScene());
	std::shared_ptr<PerspectiveCamera> camera(AG::createPerspectiveCamera(45.0f, rendererParams.aspectRatio, 0.1f, 100.0f));

	std::shared_ptr<AG::HelperArrow> arrowHelper(AG::Helper::arrow(vec3(0.0f), vec3(1.0f, 0.0f, 0.0f), 1.0f));
	std::shared_ptr<AG::HelperCameraOrientation> cameraOrientationHelper(AG::Helper::cameraOrientation(vec3(0.0f), vec3(1.0f, 0.0f, 0.0f), 1.0f));

	std::shared_ptr<AG::LightDirectional> light(AG::Light::directional(vec3(1.0f), vec3(1.0f), vec3(1.0f)));

	std::vector<Resource::File> skyboxFaces = {
		Resource::File("envmap_stormydays/stormydays_ft.tga"),
		Resource::File("envmap_stormydays/stormydays_bk.tga"),
		Resource::File("envmap_stormydays/stormydays_dn.tga"),
		Resource::File("envmap_stormydays/stormydays_up.tga"),
		Resource::File("envmap_stormydays/stormydays_rt.tga"),
		Resource::File("envmap_stormydays/stormydays_lf.tga"),
	};

	std::shared_ptr<Model> skybox(AG::Models::skybox(skyboxFaces));

	CameraControl cameraControl(camera.get());

	camera->setPosition(0.0f, 5.0f, 10.0f);
	camera->lookAt(camera->getPosition() + vec3(0.0f, 0.0f, -3.0f));

	inputHandler.setWinSize(rendererParams.width, rendererParams.height);
	inputHandler.pointerToCenter();

	light->setAmbient(0.5f, 0.5f, 0.5f);
	light->setDiffuse(0.1f, 0.05f, 0.01f);
	light->setSpecular(vec3(0.15f));
	light->setDirection(vec3(0.1f, 1.1f, 0.1f));

	Resource::File nanosuitResource("nanosuit/nanosuit.obj");
	Resource::File airboatResource("airboat.obj");
	Resource::File monkey2Resource("monkey2.obj");
	Resource::File spiderResource("spider.obj");
	Resource::File buddhaResource("buddha/buddha.obj");
	Resource::File bunnyResource("bunny/bunny.obj");
	Resource::File sponzaResource("sponza/sponza.obj");
	Resource::File sponza2Resource("sponza2/sponza.obj");
	Resource::File sponza4Resource("sponza4/sponza.obj");
	Resource::File serapisResource("serapis/serapis.stl");
	
	Resource::File linaResource("lina/lina_econ.fbx");
	Resource::File antimageResource("antimage/antimage_econ.fbx");
	Resource::File illidanResource("illidan/illidan stormrage.fbx");
	Resource::File lichKingResource("Lich_King/lich_king.fbx");
	Resource::File subzeroResource("subzero-b-posicao-vitoria/source/SubZero_B__movimentos_MKX_Android.fbx");
	Resource::File kasumiResource("kasumi-fbx-mmd-danca-dance/source/DOA5_Kasumi_MMD.fbx");
	Resource::File twobResource("2b/source/2B.fbx");
	Resource::File ironmanResource("ironman/source/TONY/IRON.obj");
	Resource::File zeusResource("zeus/zeus_econ.fbx"); // not work
	Resource::File techiesResource("techies/techies_econ.fbx");
	Resource::File sandkingResource("sand_king/sand_king_econ.fbx");
	Resource::File invokerResource("invoker/invoker_econ.fbx");
	Resource::File svenResource("sven/sven_econ.fbx");
	Resource::File mechResource("mech_ghost/source/GhostSF.FBX");
	Resource::File phantomGuardResource("phantom-guard/source/phantom-guard.fbx");
	Resource::File kiminaroResource("kiminaro/source/kiminaro.fbx");
	Resource::File kageResource("kage/source/Kage-sketfab.fbx");

	console::info("start init scene");

	scene->init();
	scene->add(arrowHelper.get());
	scene->add(cameraOrientationHelper.get());
	scene->add(camera.get());
	scene->add(light.get());
	scene->setActiveCamera(camera.get());
	
	scene->setSkybox(skybox.get());

	renderer->setScene(scene.get());
	renderer->setupScene();
	renderer->addPreRenderHandler(boost::bind(&CameraControl::update, &cameraControl));
	renderer->start();

	cameraOrientationHelper->setLength(1.0);

	renderCycle.addTickHandler([&renderer, &inputHandler, &cameraOrientationHelper, &camera, &light](float time) {
		// vec3 lightDirection = light->getDirection();
		// lightDirection.y = glm::sin(time * 0.1f) * 0.1f;

		// light->setDirection(lightDirection);
		renderer->draw();
		inputHandler.onFrame();
	});

	std::shared_ptr<Model> testModel(AG::Models::create(subzeroResource));
	testModel->scale(vec3(0.05f));
	// testModel->rotate(vec3(1.0f, 0.0f, 0.0f), -(3.14f)); // subzero
	// testModel->rotate(vec3(1.0f, 0.0f, 0.0f), -(3.14f / 2.0f)); // kasumi
	scene->add(testModel.get());

	Cycle::mailLoop();
	// don't put code below. trust me.

	console::info("stop application");

	return 0;
}
