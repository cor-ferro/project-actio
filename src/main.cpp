#include <glad/glad.h>
#include <GLFW/glfw3.h>

// #include <memory>
#include <signal.h>
#include "./app/app.h"
#include "glm/gtc/random.hpp"

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
#include "./helpers/bounding_box_helper.h"
#include "./helpers/point_light_helper.h"
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
	std::shared_ptr<Renderer::OpenglRenderer> renderer(new Renderer::OpenglRenderer(rendererParams));
	renderer->init(argc, argv);
	renderer->setTitle(app.getName().c_str());
	renderer->start();

	InputHandler &inputHandler = InputHandler::instance();

	std::shared_ptr<Scene> scene(new Scene());
	std::shared_ptr<AG::HelperArrow> arrowHelper(AG::Helper::arrow(vec3(0.0f), vec3(1.0f, 0.0f, 0.0f), 1.0f));
	std::shared_ptr<AG::HelperCameraOrientation> cameraOrientationHelper(AG::Helper::cameraOrientation(vec3(0.0f), vec3(1.0f, 0.0f, 0.0f), 1.0f));

	std::shared_ptr<Light::Point> light(AG::Light::point(vec3(1.0f), vec3(1.0f), vec3(1.0f), vec3(1.0f)));

	inputHandler.setWinSize(rendererParams.width, rendererParams.height);
	inputHandler.pointerToCenter();

	light->setAmbient(0.5f, 0.5f, 0.5f);
	light->setDiffuse(1.1f, 1.05f, 1.01f);
	light->setSpecular(vec3(0.15f));
	light->setPosition(vec3(0.0f, 10.0f, 5.0f));
	light->setAttenuation(1.0f, 0.001f, 0.012f);

	console::info("init scene");

	Resource::File testScene("testScene.ini");
	scene->initFromFile(testScene);
	// scene->add(arrowHelper.get());
	// scene->add(cameraOrientationHelper.get());

	Camera * sceneActiveCamera = scene->getActiveCamera();
	CameraControl cameraControl(sceneActiveCamera);
	sceneActiveCamera->setParam(CameraParam::ASPECT, rendererParams.aspectRatio);

	// Model * boxModel = AG::Models::box();
	Model * planeModel = AG::Models::plane(10, 10, 10, 10);
	planeModel->rotate(vec3(0.0f, 1.0f, 1.0f), glm::pi<float>());
	// Model * sphereModel = AG::Models::sphere(5.0f, 32, 32);
	// Model * circleModel = AG::Models::circle(5.0f, 12);
	// Model * cylinderModel = AG::Models::cylinder(5.0f, 5.0f, 10.0f, 16, 16);
	// Model * coneModel = AG::Models::cone(3.0f, 5.0f, 5, 16);
	// Model * torusModel = AG::Models::torus(5.0f, 1.0f, 16, 100);
	// Model * octahedronModel = AG::Models::octahedron(1.0f);

	PhongMaterial material;
	material.setDiffuse(0.0f, 1.0f, 0.0f);
	Geometry geometry = Geometry::Torus(5.0f, 1.0f, 16, 100, glm::two_pi<float>());

	for (int i = 0; i < 1; i++) {
		Mesh * mesh = new Mesh(material, geometry);
		mesh->setPosition(vec3(
			glm::gaussRand(0.0f, 7.0f),
			glm::gaussRand(0.0f, 7.0f),
			glm::gaussRand(0.0f, 7.0f)
		));
		Model * model = new Model(mesh);

		scene->add(model);
	}

	Helpers::PointLight * pointLightHelper = new Helpers::PointLight(light.get());

	Model * mainModel = scene->getModelByName("ironman");
	if (mainModel != nullptr)
	{
		Helpers::BoundingBox * boundingBoxHelper = new Helpers::BoundingBox(mainModel);
		scene->add(boundingBoxHelper);
	}

	scene->add(planeModel);
	scene->add(light.get());
	scene->add(pointLightHelper);

	cameraOrientationHelper->setLength(1.0);

	GLFWwindow * window = glfwGetCurrentContext();

	while(!glfwWindowShouldClose(window))
	{
		float time = (float)glfwGetTime();
		cameraControl.update();
		vec3 centerPoint(0.0, 10.0f, 0.0f);
		float rad = (glm::sin(time * 0.1f)) * glm::two_pi<float>();
		quat newQuat = glm::angleAxis(rad, vec3(0.0f, 1.0f, 1.0f));
		vec3 newLightPos = centerPoint * newQuat;

		light->setPosition(newLightPos);

		pointLightHelper->beforeRender();

		renderer->draw(scene.get());

		inputHandler.onFrame();

		glfwSwapBuffers(window);
		glfwPollEvents();

		if (inputHandler.isPress(InputHandler::KEY_ESC))
		{
			glfwDestroyWindow(window);
			break;
		}
	}

	console::info("stop application");

	return 0;
}
