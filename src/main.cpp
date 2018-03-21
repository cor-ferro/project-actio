#include "main.h"

int main(int argc, char **argv) {
	App& app = App::instance();
	app.setName("project actio");
	app.init(argc, argv);

	console::info("start ", app.getName());

	const size_t maxModels = 10000;
	const size_t maxMeshes = 50000;

	imageAllocator = new memory::FreeListAllocator(1e8, memory::FreeListAllocator::FIND_FIRST);
	imageAllocator->Init();

	modelsAllocator = new memory::PoolAllocator(sizeof(Model) * maxModels, sizeof(Model));
	modelsAllocator->Init();

	meshAllocator = new memory::PoolAllocator(sizeof(Mesh) * maxMeshes, sizeof(Mesh));
	meshAllocator->Init();

	const Monitor * const monitor = app.getPrimaryMonitor();
	uint windowWidth = monitor->getWidth() / 2;
	uint windowHeight = monitor->getHeight() / 2;

	WindowContext mainContext;
	
	if (!mainContext.init(windowWidth, windowHeight)) {
		return -1;
	}

	mainContext.setTitle(app.getName().c_str());

	RendererParams rendererParams;
	rendererParams.width = monitor->getWidth();
	rendererParams.height = monitor->getHeight();
	rendererParams.calcAspectRatio();

	// init renderer before scene loading
	Renderer::OpenglRenderer * renderer = new Renderer::OpenglRenderer(rendererParams);
	renderer->setShadersFolder(app.shadersPath());
	renderer->setType(Renderer::OpenglRenderer::RenderDeferred);
	renderer->init(argc, argv);
	renderer->start();

	InputHandler inputHandler(mainContext);

	printMemoryStatus();

	// Camera * sceneActiveCamera = scene->getActiveCamera();
	// CameraControl cameraControl(sceneActiveCamera, &inputHandler);

	// cameraControl.calcSensetivity(monitor->getWidth(), monitor->getHeight(), monitor->getDpi());
	// sceneActiveCamera->setParam(CameraParam::ASPECT, rendererParams.aspectRatio);

	// Material::Phong material;
	// material.setDiffuse(0.0f, 1.0f, 0.0f);
	// Geometry geometry = Geometry::Torus(5.0f, 1.0f, 16, 100, glm::two_pi<float>());

	// for (int i = 0; i < 1; i++) {
	// 	Mesh * mesh = Mesh::Create(geometry, material);
	// 	mesh->setPosition(vec3(
	// 		glm::gaussRand(0.0f, 7.0f),
	// 		glm::gaussRand(0.0f, 7.0f),
	// 		glm::gaussRand(0.0f, 7.0f)
	// 	));
	// 	Model * model = Model::Create(mesh);

	// 	scene->add(model);
	// }

	// Model * planeModel = AG::Models::plane(10, 10, 10, 10);
	// planeModel->rotate(vec3(0.0f, 1.0f, 1.0f), glm::pi<float>());
	// scene->add(planeModel);

	// Model * sphereModel = AG::Models::sphere(3.0f, 16, 16);
	// scene->add(sphereModel);

	const Resource::File testWorldFile = app.resource("testScene.ini");

	game::World* world = game::createWorld();
	world->initFromFile(testWorldFile);

	GLFWwindow * const window = mainContext.getWindow();

	mainContext.setAsCurrent();
	mainContext.enableVSync();

	bool execAppLoop = true;
	bool execRenderLoop = true;

	std::thread([&]() {
		while(execRenderLoop) {
			const float elapsedTime = 16.6f;

			// cameraControl.update();
			inputHandler.onFrame();

			// const std::vector<Model*>& models = scene->getModels();

			// for (Model * model : models) {
			// 	model->tickAnimationTime(elapsedTime / 1000.0f);
			// 	model->processAnimation();
			// }

			world->update(elapsedTime);

			std::this_thread::sleep_for(std::chrono::microseconds(16600));
		}

		execAppLoop = false;
	}).detach();

	while(!glfwWindowShouldClose(window))
	{
		float elapsedTime = static_cast<float>(glfwGetTime());
		// renderer->draw(scene);
		world->render(elapsedTime);

		{
			ImGui_ImplGlfwGL3_NewFrame();
			ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), true);

			ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoInputs|ImGuiWindowFlags_NoBringToFrontOnFocus;

			ImGui::Begin("Metrics", NULL, flags);
			ImGui::Text("%.3f ms, %.1f fps", renderer->stats.msFrame, renderer->stats.fps);
			ImGui::Text("images: %s", utils::formatMemorySize(imageAllocator->getUsed()).c_str());
			ImGui::Text("models: %s", utils::formatMemorySize(modelsAllocator->getUsed()).c_str());
			ImGui::Text("meshes: %s", utils::formatMemorySize(meshAllocator->getUsed()).c_str());

			ImGui::SetWindowSize(ImVec2(200.0f, 80.0f));
			ImGui::End();
			ImGui::Render();
		}
		

		glfwSwapBuffers(window);
		glfwPollEvents();

		if (inputHandler.isPress(InputHandler::KEY_ESC))
		{
			mainContext.destroy();
			execRenderLoop = false;
			break;
		}
	}

	// wait all loops finish
	while (execRenderLoop || execAppLoop) {
		continue;
	}

	world->destroy();

	// cleanupScene(scene);
	printMemoryStatus();

	delete imageAllocator;
	delete modelsAllocator;
	delete meshAllocator;

	// delete scene;
	delete world;
	delete renderer;

	console::info("stop application");

	return 0;
}

// void cleanupScene(Scene * scene)
// {
// 	std::vector<Model*> sceneModels = scene->getModels();

// 	for (uint i = 0; i < sceneModels.size(); i++) {
// 		Model * model = sceneModels[i];
// 		Model::Destroy(model);
// 	}
// }

void printMemoryStatus()
{
	console::info("-------memory-------");
	console::info("images: ", utils::formatMemorySize(imageAllocator->getUsed()));
	console::info("models: ", utils::formatMemorySize(modelsAllocator->getUsed()));
	console::info("mesh: ", utils::formatMemorySize(meshAllocator->getUsed()));
	console::info("--------------------");
}