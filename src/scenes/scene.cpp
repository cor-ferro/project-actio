#include "scene.h"
#include "../lib/ini_loader.h"

Scene::Scene()
{
	allocCameras(5);
	allocLights(5);
}

Scene::Scene(const Scene& other)
{
	console::info("copy scene");
	skybox_ = other.skybox_;
	models_ = other.models_;
	cameras_ = other.cameras_;
	camera_ = other.camera_;

	directionalLights_ = other.directionalLights_;
	pointLights_ = other.pointLights_;
	spotLights_ = other.spotLights_;
}

Scene::~Scene()
{
	console::info("destroy scene");
}

bool Scene::init()
{
	allocModels(0);
	return true;
}

const std::vector<Model*>& Scene::getModels()
{
	return models_;
}

Model * Scene::getModelByName(std::string name)
{
	for (auto model : models_) {
		if (model->getName() == name) return model;
	}

	return nullptr;
}

void Scene::allocModels(unsigned int count)
{
	models_.reserve(count);
}

void Scene::allocCameras(unsigned int count)
{
	cameras_.reserve(count);
}

void Scene::allocLights(unsigned int count)
{
	directionalLights_.reserve(count);
	pointLights_.reserve(count);
	spotLights_.reserve(count);
}

Model * Scene::getSkybox()
{
	return skybox_;
}

void Scene::setSkybox(Model * model)
{
	skybox_ = model;
}

bool Scene::hasSkybox()
{
	return skybox_ != nullptr;
}

Camera * Scene::getActiveCamera()
{
	return camera_;
}

void Scene::setActiveCamera(Camera * activeCamera)
{
	camera_ = activeCamera;
}

void Scene::add(Model * model) { models_.push_back(model); }
void Scene::add(std::shared_ptr<Model> model) { models_.push_back(model.get()); }
void Scene::add(Camera * camera) { cameras_.push_back(camera); }

void Scene::remove(Model * model)
{
	models_.erase(std::remove(models_.begin(), models_.end(), model), models_.end()); 
}

void Scene::remove(Camera * camera)
{
	cameras_.erase(std::remove(cameras_.begin(), cameras_.end(), camera), cameras_.end()); 
}

/* ---------- Lights ---------- */

const std::vector<Light::Directional*>& Scene::getDirectionalLights()
{
	return directionalLights_;
}

const std::vector<Light::Point*>& Scene::getPointLights()
{
	return pointLights_;
}

const std::vector<Light::Spot*>& Scene::getSpotLights()
{
	return spotLights_;
}

Light::Directional * Scene::getDirectionalLight(uint index)
{
	if (index >= directionalLights_.size()) return nullptr;

	return directionalLights_.at(index);
}

Light::Point * Scene::getPointLight(uint index)
{
	if (index >= pointLights_.size()) return nullptr;

	return pointLights_.at(index);
}

Light::Spot * Scene::getSpotLight(uint index)
{
	if (index >= spotLights_.size()) return nullptr;

	return spotLights_.at(index);
}

void Scene::add(Light::Directional * light) { directionalLights_.push_back(light); }
void Scene::add(Light::Point * light) { pointLights_.push_back(light); }
void Scene::add(Light::Spot * light) { spotLights_.push_back(light); }

void Scene::remove(Light::Directional * light)
{
	directionalLights_.erase(
		std::remove(directionalLights_.begin(), directionalLights_.end(), light), directionalLights_.end()
	);
}

void Scene::remove(Light::Point *light)
{
	pointLights_.erase(
		std::remove(pointLights_.begin(), pointLights_.end(), light), pointLights_.end()
	);
}

void Scene::remove(Light::Spot *light)
{
	spotLights_.erase(
		std::remove(spotLights_.begin(), spotLights_.end(), light), spotLights_.end()
	);
}
