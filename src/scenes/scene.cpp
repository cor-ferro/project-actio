#include "scene.h"

vec3 parseVec(const char * chars) {
	std::string str(chars);
	const char delimiter = ',';
	std::string part = "";
	std::vector<std::string> parts;	

	for (auto it = str.begin(); it != str.end(); it++) {
		if ((*it) != delimiter) {
			part+= (*it);
		} else {
			parts.push_back(part);
			part = "";
		}
	}

	if (part != "") {
		parts.push_back(part);
	}

	if (parts.size() == 1) {
		return vec3(std::stof(parts[0]));
	} else if (parts.size() == 3) {
		
		return vec3(
			std::stof(parts[0]),
			std::stof(parts[1]),
			std::stof(parts[2])
		);
	} else {
		return vec3(0.0f);
	}

	return vec3(0.0f);
}

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

bool Scene::initFromFile(Resource::File& file)
{
	dictionary * ini = iniparser_load(file.getPath().c_str());
	
	const char * iniModels = iniparser_getstring(ini, "scene:models", "");
	const char * iniLights = iniparser_getstring(ini, "scene:lights", "");
	const char * iniSkybox = iniparser_getstring(ini, "scene:skybox", "");
	const char * iniCamera = iniparser_getstring(ini, "scene:camera", "");
	
	console::info(" ");

	std::vector<ModelConfig> * modelConfigs = new std::vector<ModelConfig>();
	Model * skyboxModel = nullptr;

	std::istringstream models(iniModels);
	std::string model;
	while (getline(models, model, ' ')) {
		console::info("scene model: ", model);
		std::string iniModelSection("model.");
		iniModelSection+= model;

		int modelFound = iniparser_find_entry(ini, iniModelSection.c_str());

		if (modelFound) {
			std::string modelFileKey = iniModelSection + ":File";
			std::string modelFlipUvKey = iniModelSection + ":FlipUv";
			std::string modelPositionKey = iniModelSection + ":Position";
			std::string modelRotationKey = iniModelSection + ":Rotation";
			std::string modelRotationAngleKey = iniModelSection + ":RotationAngle";
			std::string modelScaleKey = iniModelSection + ":Scale";
			std::string modelAnimationKey = iniModelSection + ":Animation";

			const char * iniModelFile = iniparser_getstring(ini, modelFileKey.c_str(), "");
			const int iniModelFlipUv = iniparser_getboolean(ini, modelFlipUvKey.c_str(), 0);
			const char * iniModelPosition = iniparser_getstring(ini, modelPositionKey.c_str(), "0.0");
			const char * iniModelRotation = iniparser_getstring(ini, modelRotationKey.c_str(), "1.0");
			const double iniModelRotationAngle = iniparser_getdouble(ini, modelRotationAngleKey.c_str(), 0.0);
			const char * iniModelScale = iniparser_getstring(ini, modelScaleKey.c_str(), "1.0");
			const char * iniModelAnimation = iniparser_getstring(ini, modelAnimationKey.c_str(), "");

			ModelConfig modelConfig;
			modelConfig.name = model;
			modelConfig.file = Resource::File(iniModelFile);
			modelConfig.flipUv = iniModelFlipUv;
			modelConfig.position = parseVec(iniModelPosition);
			modelConfig.rotation = parseVec(iniModelRotation);
			modelConfig.rotationAngle = (float)iniModelRotationAngle;
			modelConfig.scale = parseVec(iniModelScale);
			modelConfig.animation = std::string(iniModelAnimation);

			modelConfigs->push_back(modelConfig);
		} else {
			console::warn("ini model not found: ", iniModelSection);
		}
	}

	unsigned int modelsCount = 0;
	modelsCount+= modelConfigs->size();

	allocModels(modelsCount);

	for (auto it = modelConfigs->begin(); it != modelConfigs->end(); it++) {
		ModelConfig& modelConfig = (*it);
		Model * model = AG::Models::create(modelConfig); // @todo: удалять модель

		add(model);
	}

	std::string skybox(iniSkybox);
	if (skybox != "") {
		std::string skyboxSection("skybox.");
		skyboxSection+= skybox;

		int skyboxFound = iniparser_find_entry(ini, skyboxSection.c_str());

		if (skyboxFound) {
			std::string iniSkyboxPositiveXKey = skyboxSection + ":PositiveX";
			std::string iniSkyboxNegativeXKey = skyboxSection + ":NegativeX";
			std::string iniSkyboxPositiveYKey = skyboxSection + ":PositiveY";
			std::string iniSkyboxNegativeYKey = skyboxSection + ":NegativeY";
			std::string iniSkyboxPositiveZKey = skyboxSection + ":PositiveZ";
			std::string iniSkyboxNegativeZKey = skyboxSection + ":NegativeZ";

			const char * iniSkyboxPositiveX = iniparser_getstring(ini, iniSkyboxPositiveXKey.c_str(), "");
			const char * iniSkyboxNegativeX = iniparser_getstring(ini, iniSkyboxNegativeXKey.c_str(), "");
			const char * iniSkyboxPositiveY = iniparser_getstring(ini, iniSkyboxPositiveYKey.c_str(), "");
			const char * iniSkyboxNegativeY = iniparser_getstring(ini, iniSkyboxNegativeYKey.c_str(), "");
			const char * iniSkyboxPositiveZ = iniparser_getstring(ini, iniSkyboxPositiveZKey.c_str(), "");
			const char * iniSkyboxNegativeZ = iniparser_getstring(ini, iniSkyboxNegativeZKey.c_str(), "");

			std::vector<Resource::File> skyboxFaces = {
				Resource::File(iniSkyboxPositiveX),
				Resource::File(iniSkyboxNegativeX),
				Resource::File(iniSkyboxPositiveY),
				Resource::File(iniSkyboxNegativeY),
				Resource::File(iniSkyboxPositiveZ),
				Resource::File(iniSkyboxNegativeZ),
			};

			skyboxModel = AG::Models::skybox(skyboxFaces); // @todo: удаление
			setSkybox(skyboxModel);
		}
	}

	std::string camera(iniCamera);
	if (camera != "") {
		std::string cameraSection("camera.");
		cameraSection+= camera;

		int cameraFound = iniparser_find_entry(ini, cameraSection.c_str());

		if (cameraFound) {
			std::string iniCameraTypeKey = cameraSection + ":Type";
			std::string iniCameraPositionKey = cameraSection + ":Position";
			std::string iniCameraTargetKey = cameraSection + ":Target";
			std::string iniCameraNearKey = cameraSection + ":Near";
			std::string iniCameraFarKey = cameraSection + ":Far";
			const char * iniCameraType = iniparser_getstring(ini, iniCameraTypeKey.c_str(), "");
			const char * iniCameraPosition = iniparser_getstring(ini, iniCameraPositionKey.c_str(), "0.0");
			const char * iniCameraTarget = iniparser_getstring(ini, iniCameraTargetKey.c_str(), "-3.0");
			const double iniCameraNear = iniparser_getdouble(ini, iniCameraNearKey.c_str(), 0.1);
			const double iniCameraFar = iniparser_getdouble(ini, iniCameraFarKey.c_str(), 100.0);

			if (strcmp(iniCameraType, "perspective") == 0) {
				std::string iniCameraFovKey = cameraSection + ":Fov";
				std::string iniCameraAspectRatioKey = cameraSection + ":AspectRatio";

				const double iniCameraFov = iniparser_getdouble(ini, iniCameraFovKey.c_str(), 45.0);
				const double iniCameraAspectRatio = iniparser_getdouble(ini, iniCameraAspectRatioKey.c_str(), 1.77);
				

				PerspectiveCamera * camera = AG::createPerspectiveCamera( // @todo: удалять
					(float)iniCameraFov, 
					(float)iniCameraAspectRatio, 
					(float)iniCameraNear, 
					(float)iniCameraFar
				);

				camera->setPosition(parseVec(iniCameraPosition));
				camera->lookAt(camera->getPosition() + parseVec(iniCameraTarget));

				setActiveCamera(camera);
			}
			else if (strcmp(iniCameraType, "ortho") == 0) {
				std::string iniCameraLeftKey = cameraSection + ":Left";
				std::string iniCameraRightKey = cameraSection + ":Right";
				std::string iniCameraTopKey = cameraSection + ":Top";
				std::string iniCameraBottomKey = cameraSection + ":Bottom";

				const double iniCameraLeft = iniparser_getdouble(ini, iniCameraLeftKey.c_str(), 45.0);
				const double iniCameraRight = iniparser_getdouble(ini, iniCameraRightKey.c_str(), 1.77);
				const double iniCameraTop = iniparser_getdouble(ini, iniCameraTopKey.c_str(), 0.1);
				const double iniCameraBottom = iniparser_getdouble(ini, iniCameraBottomKey.c_str(), 100.0);

				OrthographicCamera * camera = AG::createOrthographicCamera( // @todo: удалять
					(float)iniCameraLeft,
					(float)iniCameraRight,
					(float)iniCameraTop,
					(float)iniCameraBottom,
					(float)iniCameraNear, 
					(float)iniCameraFar
				);

				camera->setPosition(parseVec(iniCameraPosition));
				camera->lookAt(camera->getPosition() + parseVec(iniCameraTarget));

				setActiveCamera(camera);
			} else {
				console::warn("unsupported camera type: ", iniCameraType);
			}
		} else {
			console::warn("required scene camera");
		}
	}

	std::istringstream lights(iniLights);
	std::string light;
	while (getline(lights, light, ' ')) {
		console::info("scene light: ", light);
		std::string iniLightSection("light.");
		iniLightSection+= light;

		int lightFound = iniparser_find_entry(ini, iniLightSection.c_str());
		if (lightFound) {
			std::string lightTypeKey = iniLightSection + ":Type";
			std::string lightAmbientKey = iniLightSection + ":Ambient";
			std::string lightDiffuseKey = iniLightSection + ":Diffuse";
			std::string lightSpecularKey = iniLightSection + ":Specular";
			
			const char * iniLightType = iniparser_getstring(ini, lightTypeKey.c_str(), "");
			const char * iniLightAmbient = iniparser_getstring(ini, lightAmbientKey.c_str(), "1.0");
			const char * iniLightDiffuse = iniparser_getstring(ini, lightDiffuseKey.c_str(), "1.0");
			const char * iniLightSpecular = iniparser_getstring(ini, lightSpecularKey.c_str(), "1.0");

			if (strcmp(iniLightType, "directional") == 0) {
				std::string lightDirectionKey = iniLightSection + ":Direction";
				const char * iniLightDirection = iniparser_getstring(ini, lightDirectionKey.c_str(), "1.0");

				Light::Directional * light = AG::Light::directional(vec3(1.0f), vec3(1.0f), vec3(1.0f)); // @todo: удалять

				light->setAmbient(parseVec(iniLightAmbient));
				light->setDiffuse(parseVec(iniLightDiffuse));
				light->setSpecular(parseVec(iniLightSpecular));
				light->setDirection(parseVec(iniLightDirection));

				add(light);
			} else if (strcmp(iniLightType, "point") == 0) {
				std::string lightPositionKey = iniLightSection + ":Position";
				std::string lightConstantKey = iniLightSection + ":Constant";
				std::string lightLinearKey = iniLightSection + ":Linear";
				std::string lightQuadraticKey = iniLightSection + ":Quadratic";
				const char * iniLightPosition = iniparser_getstring(ini, lightPositionKey.c_str(), "1.0");
				const double iniLightConstant = iniparser_getdouble(ini, lightConstantKey.c_str(), 1.0);
				const double iniLightLinear = iniparser_getdouble(ini, lightLinearKey.c_str(), 0.09);
				const double iniLightQuadratic = iniparser_getdouble(ini, lightQuadraticKey.c_str(), 0.032);

				Light::Point * light = AG::Light::point(vec3(1.0f), vec3(1.0f), vec3(1.0f), vec3(1.0f)); // @todo: удалять
				light->setAmbient(parseVec(iniLightAmbient));
				light->setDiffuse(parseVec(iniLightDiffuse));
				light->setSpecular(parseVec(iniLightSpecular));
				light->setPosition(parseVec(iniLightPosition));
				light->setAttenuation((float)iniLightConstant, (float)iniLightLinear, (float)iniLightQuadratic);

				add(light);
			} else if (strcmp(iniLightType, "spot") == 0) {
				std::string lightPositionKey = iniLightSection + ":Position";
				std::string lightDirectionKey = iniLightSection + ":Direction";
				std::string lightConstantKey = iniLightSection + ":Constant";
				std::string lightLinearKey = iniLightSection + ":Linear";
				std::string lightQuadraticKey = iniLightSection + ":Quadratic";
				std::string lightCutOffKey = iniLightSection + ":CutOff";
				std::string lightOuterCutOff = iniLightSection + ":OuterCutOff";
				const char * iniLightPosition = iniparser_getstring(ini, lightPositionKey.c_str(), "1.0");
				const char * iniLightDirection = iniparser_getstring(ini, lightDirectionKey.c_str(), "1.0");
				const double iniLightConstant = iniparser_getdouble(ini, lightConstantKey.c_str(), 1.0);
				const double iniLightLinear = iniparser_getdouble(ini, lightLinearKey.c_str(), 0.09);
				const double iniLightQuadratic = iniparser_getdouble(ini, lightQuadraticKey.c_str(), 0.22);
				const double iniLightOuterCutOff = iniparser_getdouble(ini, lightOuterCutOff.c_str(), 0.82);

				Light::Spot * light = AG::Light::spot(vec3(1.0f), vec3(1.0f), vec3(1.0f)); // @todo: удалять
				light->setAmbient(parseVec(iniLightAmbient));
				light->setDiffuse(parseVec(iniLightDiffuse));
				light->setSpecular(parseVec(iniLightSpecular));
				light->setPosition(parseVec(iniLightPosition));
				light->setDirection(parseVec(iniLightPosition));
				light->setAttenuation((float)iniLightConstant, (float)iniLightLinear, (float)iniLightQuadratic);
				light->setCutoff((float)iniLightQuadratic, (float)iniLightOuterCutOff);

				add(light);
			} else {
				console::warn("unsupported light type: ", iniLightType);
			}
		}
	}

	iniparser_freedict(ini);

	return true;
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
