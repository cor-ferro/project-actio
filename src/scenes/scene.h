#ifndef SCENE_H_
#define SCENE_H_

#include <memory>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
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

    Scene(const Scene &other);

    ~Scene();

    bool init();

    const std::vector<Model *> &getModels();

    Model *getModelByName(ModelName name);

    Model *getModelById(ModelId id);

    void allocModels(unsigned int count);

    void allocCameras(unsigned int count);

    void allocLights(unsigned int count);

    Camera *getActiveCamera();

    void setActiveCamera(Camera *);

    void add(Model *);

    void add(std::shared_ptr<Model>);

    void add(Camera *);

    void remove(Model *);

    void remove(Camera *);

    Model *getSkybox();

    void setSkybox(Model *);

    bool hasSkybox();

    const std::vector<Light::Directional *> &getDirectionalLights();

    const std::vector<Light::Point *> &getPointLights();

    const std::vector<Light::Spot *> &getSpotLights();

    Light::Directional *getDirectionalLight(uint index);

    Light::Point *getPointLight(uint index);

    Light::Spot *getSpotLight(uint index);

    void add(Light::Directional *);

    void add(Light::Point *);

    void add(Light::Spot *);

    void remove(Light::Directional *);

    void remove(Light::Point *);

    void remove(Light::Spot *);

private:
    Model *skybox_ = nullptr;
    std::vector<Model *> models_;
    std::vector<Camera *> cameras_;

    std::vector<Light::Directional *> directionalLights_;
    std::vector<Light::Point *> pointLights_;
    std::vector<Light::Spot *> spotLights_;

    Camera *camera_;
};

vec3 parseVec(const char *str);

#endif
