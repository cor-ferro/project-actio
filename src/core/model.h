#ifndef MODEL_H_
#define MODEL_H_

#include <memory>
#include <boost/thread.hpp>
#include <unordered_map>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../lib/console.h"
#include "../lib/types.h"
#include "../lib/assimp.h"
#include "../resources/resources.h"
#include "../animation/animation.h"
#include "../math/Box3.h"
#include "./mesh.h"

typedef int ModelId;
typedef std::string ModelName;
typedef std::vector<std::shared_ptr<Mesh>> ModelMeshes;

static size_t idCounter = 0;
static ModelId newId()
{
	return ++idCounter;
}

struct ModelConfig {
	ModelConfig();
	ModelConfig(const ModelConfig& other);

	std::string name;
	Resource::File file;
	bool flipUv;
	vec3 position;
	vec3 rotation;
	float rotationAngle;
	vec3 scale;
	std::string animation;
};

struct ModelNode {
	ModelNode();
	ModelNode(aiNode * node);
	~ModelNode();

	void addMesh(Mesh * mesh);
	void addNode(ModelNode * node);

	std::string name;
	std::vector<std::shared_ptr<ModelNode>> children;
	std::vector<std::shared_ptr<Mesh>> meshes;
	mat4 transformation;
};

struct Model {
	Model();
	// Model(Resource::File);
	Model(ModelConfig& modelConfig);
	Model(Mesh * mesh);
	Model(const Model& model);
	~Model();

	const ModelId& getId();
	const ModelName& getName();

	void setName(std::string newName);

	void initFromAi(const Resource::Assimp * assimpResource);
	void addMesh(Mesh * mesh);
	void addNode(ModelNode * node);
	
	void setScale(vec3 scale);
	void rotate(vec3 rotate, float angle);
	void setPosition(vec3 position);
	void setPosition(float x, float y, float z);
	void setQuaternion(float x, float y, float z, float w);

	void setCurrentAnimation(std::string animationName);
	void addAnimation(const Animation *);
	const Animation * const getAnimation(const std::string& name);
	void processCurrentAnimation();
	void processAnimation(AnimationProcess& animationProcess);
	void processNodeAnimation(ModelNode * node, const Animation * const animation, mat4& transform, double tick);

	void enableAnimIterpolation();
	void disableAnimIterpolation();

	const ModelMeshes& getMeshes();
	Mesh * getFirstMesh();

	AnimationProcess currentAnimation;
	mat4 GlobalInverseTransform;
private:
	void processNode(aiNode * node, ModelNode * modelNode, const Resource::Assimp * assimpResource);

	void allocMeshes(unsigned int);
	void freeMeshes();
	void freeNodes();

	std::vector<aiNode*> collectAiNodes(aiNode*);
	void collectAiNode(aiNode*, std::vector<aiNode*>& nodes);

	ModelId id_;
	ModelName name_;
	ModelMeshes meshes_;
	ModelNode * rootNode_;
	bool animInterpolation_;
	std::unordered_map<std::string, std::shared_ptr<const ModelNode>> nodes_;
	std::unordered_map<std::string, std::shared_ptr<const Animation>> animations_;
};

#endif
