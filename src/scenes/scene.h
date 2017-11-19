#ifndef SCENE_H_
#define SCENE_H_

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include "iniparser/src/iniparser.h"
#include "iniparser/src/dictionary.h"
#include "../ag.h"
#include "../resources/resources.h"
#include "../core/model.h"
#include "../cameras/camera.h"
#include "../lights/direction_light.h"
#include "../lights/point_light.h"
#include "../lights/spot_light.h"
#include "../lib/console.h"
#include "../lib/types.h"
#include "../app/app.h"
#include "../core/model.h"
#include "../core/mesh.h"

struct Scene {
	Scene();

	bool initFromFile(Resource::File& file);
	bool init();
	std::vector<Model*>* getModels();
	
	void allocModels(unsigned int count);
	void allocCameras(unsigned int count);
	void allocLights(unsigned int count);

	Camera * getActiveCamera();
	void setActiveCamera(Camera *);

	void add(Model *);
	void add(Camera *);
	void remove(Model *);
	void remove(Camera *);

	Model * getSkybox();
	void setSkybox(Model *);
	bool hasSkybox();

	std::vector<Renderer::Light::Directional*> * getDirectionalLights();
	std::vector<Renderer::Light::Point*> * getPointLights();
	std::vector<Renderer::Light::Spot*> * getSpotLights();

	void add(Renderer::Light::Directional *);
	void add(Renderer::Light::Point *);
	void add(Renderer::Light::Spot *);
	void remove(Renderer::Light::Directional *);
	void remove(Renderer::Light::Point *);
	void remove(Renderer::Light::Spot *);

private:
	Model * skybox_ = nullptr;
	std::vector<Model*> * models_;
	std::vector<Camera*> * cameras_;

	std::vector<Renderer::Light::Directional*> * directionalLights_;
	std::vector<Renderer::Light::Point*> * pointLights_;
	std::vector<Renderer::Light::Spot*> * spotLights_;

	Camera * camera_;
};

vec3 parseVec(const char * str);

#endif
