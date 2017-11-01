#include "scene.h"

Scene::Scene()
{
	allocCameras(5);
	allocLights(5);
}

bool Scene::testInit()
{
//	const std::string pFile = App::instance().resourcePath("nanosuit/nanosuit.obj");
//	const std::string pFile = App::instance().resourcePath("airboat.obj");
//	const std::string pFile = App::instance().resourcePath("monkey2.obj");
//	const std::string pFile = App::instance().resourcePath("spider.obj");
//	const std::string pFile = App::instance().resourcePath("buddha/buddha.obj");
//	const std::string pFile = App::instance().resourcePath("bunny/bunny.obj");
//	const std::string pFile = App::instance().resourcePath("sponza/sponza.obj");
//	const std::string pFile = App::instance().resourcePath("sponza2/sponza.obj");
//	const std::string pFile = App::instance().resourcePath("serapis/serapis.stl");

//	console::info("pFile", pFile);
//
//	Assimp::Importer importer;
//	SceneContextLoader *sceneContextLoader = new SceneContextLoader();
//
//	console::info("start read scene");
//
//	const aiScene* scene = importer.ReadFile(pFile,
//		aiProcess_CalcTangentSpace |
//		aiProcess_Triangulate |
//		aiProcess_JoinIdenticalVertices |
//		aiProcess_SortByPType |
//		aiProcess_GenNormals
//	);
//
//	console::info("scene ready");
//
//	if(!scene)
//	{
//		console::err(importer.GetErrorString());
//		return false;
//	}
//
////	sceneContextLoader->setScene(scene);
//	sceneContextLoader->setBaseFilePath(App::instance().resourcePath("nanosuit"));
//
//	allocModels(1);
//
//	Model * modelBox = new Model(scene);
//	add(modelBox);
//
//	importer.FreeScene();
//
//	console::info("scene test init success");

	init();

	return true;
}

bool Scene::init()
{
	allocModels(0);
	return true;
}

std::vector<Model*> * Scene::getModels()
{
	return models_;
}

void Scene::allocModels(unsigned int count)
{
	models_ = new std::vector<Model*>();
	models_->reserve(count);
}

void Scene::allocCameras(unsigned int count)
{
	cameras_ = new std::vector<Camera*>();
	cameras_->reserve(count);
}

void Scene::allocLights(unsigned int count)
{
	directionalLights_ = new std::vector<Renderer::Light::Directional*>();
	directionalLights_->reserve(count);

	pointLights_ = new std::vector<Renderer::Light::Point*>();
	pointLights_->reserve(count);

	spotLights_ = new std::vector<Renderer::Light::Spot*>();
	spotLights_->reserve(count);
}

Camera * Scene::getActiveCamera()
{
	return camera_;
}

void Scene::setActiveCamera(Camera * activeCamera)
{
	camera_ = activeCamera;
}

void Scene::add(Model * model) { models_->push_back(model); }
void Scene::add(Camera * camera) { cameras_->push_back(camera); }

void Scene::remove(Model * model)
{
	models_->erase(std::remove(models_->begin(), models_->end(), model), models_->end()); 
}

void Scene::remove(Camera * camera)
{
	cameras_->erase(std::remove(cameras_->begin(), cameras_->end(), camera), cameras_->end()); 
}

/* ---------- Lights ---------- */

std::vector<Renderer::Light::Directional*> * Scene::getDirectionalLights()
{
	return directionalLights_;
}

std::vector<Renderer::Light::Point*> * Scene::getPointLights()
{
	return pointLights_;
}

std::vector<Renderer::Light::Spot*> * Scene::getSpotLights()
{
	return spotLights_;
}

void Scene::add(Renderer::Light::Directional *light) { directionalLights_->push_back(light); }
void Scene::add(Renderer::Light::Point *light) { pointLights_->push_back(light); }
void Scene::add(Renderer::Light::Spot *light) { spotLights_->push_back(light); }

void Scene::remove(Renderer::Light::Directional * light)
{
	directionalLights_->erase(
		std::remove(directionalLights_->begin(), directionalLights_->end(), light), directionalLights_->end()
	);
}

void Scene::remove(Renderer::Light::Point *light)
{
	pointLights_->erase(
		std::remove(pointLights_->begin(), pointLights_->end(), light), pointLights_->end()
	);
}

void Scene::remove(Renderer::Light::Spot *light)
{
	spotLights_->erase(
		std::remove(spotLights_->begin(), spotLights_->end(), light), spotLights_->end()
	);
}
