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

int main(int argc, char **argv) {
	App& app = App::instance();
	app.setName("project actio");
	app.init(argc, argv);

	console::info("start ", app.getName());

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

	std::shared_ptr<Scene> scene(new Scene());
	std::shared_ptr<AG::HelperArrow> arrowHelper(AG::Helper::arrow(vec3(0.0f), vec3(1.0f, 0.0f, 0.0f), 1.0f));
	std::shared_ptr<AG::HelperCameraOrientation> cameraOrientationHelper(AG::Helper::cameraOrientation(vec3(0.0f), vec3(1.0f, 0.0f, 0.0f), 1.0f));

	std::shared_ptr<AG::LightDirectional> light(AG::Light::directional(vec3(1.0f), vec3(1.0f), vec3(1.0f)));

	inputHandler.setWinSize(rendererParams.width, rendererParams.height);
	inputHandler.pointerToCenter();

	light->setAmbient(0.5f, 0.5f, 0.5f);
	light->setDiffuse(0.1f, 0.05f, 0.01f);
	light->setSpecular(vec3(0.15f));
	light->setDirection(vec3(0.1f, 1.1f, 0.1f));

	console::info("init scene");

	Resource::File testScene("testScene.ini");
	scene->initFromFile(testScene);
	scene->add(arrowHelper.get());
	scene->add(cameraOrientationHelper.get());

	Camera * sceneActiveCamera = scene->getActiveCamera();
	CameraControl cameraControl(sceneActiveCamera);
	sceneActiveCamera->setParam(CameraParam::ASPECT, rendererParams.aspectRatio);
	renderer->setScene(scene.get());
	renderer->setupScene();
	renderer->addPreRenderHandler(boost::bind(&CameraControl::update, &cameraControl));
	renderer->start();

	Model * boxModel = AG::Models::box();
	Model * planeModel = AG::Models::plane(1, 1, 5, 5);
	Model * sphereModel = AG::Models::sphere(5.0f, 32, 32);
	Model * circleModel = AG::Models::circle(5.0f, 12);
	Model * cylinderModel = AG::Models::cylinder(5.0f, 5.0f, 10.0f, 16, 16);
	Model * coneModel = AG::Models::cone(3.0f, 5.0f, 5, 16);
	Model * ringModel = AG::Models::ring(2.0f, 5.0f, 16);
	Model * torusModel = AG::Models::torus(5.0f, 1.0f, 16, 100);
	Model * octahedronModel = AG::Models::octahedron(1.0f);

	// scene->add(planeModel);
	// scene->add(boxModel);
	// scene->add(sphereModel);
	// scene->add(circleModel);
	// scene->add(cylinderModel);
	// scene->add(coneModel);
	// scene->add(ringModel);
	// scene->add(torusModel);
	scene->add(octahedronModel);

	cameraOrientationHelper->setLength(1.0);

	renderCycle.addTickHandler([&renderer, &inputHandler, &cameraOrientationHelper, &sceneActiveCamera, &light](float time) {
		renderer->draw();
		inputHandler.onFrame();
	});

	Cycle::mailLoop();
	// don't put code below. trust me.

	console::info("stop application");

	return 0;
}
