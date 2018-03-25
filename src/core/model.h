#ifndef MODEL_H_
#define MODEL_H_

#include <memory>
#include <unordered_map>
#include <map>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <ozz/base/memory/allocator.h>
#include <ozz/animation/runtime/skeleton.h>
#include <ozz/animation/offline/raw_skeleton.h>
#include <ozz/animation/offline/skeleton_builder.h>
#include "../lib/types.h"
#include "../lib/image_loader.h"
#include "../lib/comparators.h"
#include "../resources/resources.h"
#include "../animation/animation.h"
#include "../animation/process.h"
#include "./object3D.h"
#include "./mesh.h"

#include "../memory/poolallocator.h"

using ozz::animation::offline::RawSkeleton;

extern memory::PoolAllocator * modelsAllocator;

typedef std::size_t ModelId;
typedef std::string ModelName;
typedef std::vector<Mesh*> ModelMeshes;

static size_t idCounter = 0;
static ModelId newId()
{
	return ++idCounter;
}

struct Model {
	struct Config {
		Config();
		Config(const Config& other);

		std::string name;
		Resource::File file;
		bool flipUv;
		vec3 position;
		vec3 rotation;
		float rotationAngle;
		vec3 scale;
		std::string animation;
	};

	struct Node {
		Node();
		Node(aiNode * node);
		~Node();

		void addMesh(Mesh * mesh);
		void addNode(Node * node);

		std::string name;
		std::vector<std::shared_ptr<Node>> children;
		std::vector<Mesh*> meshes;
		mat4 transformation;
	};

	struct BoneMap {
		uint jointIndex;
		uint boneIndex;
		mat4 offset;
	};

	Model();
    Model(const Model& model);
    Model(const Model* model);

	static Model * Create();
	static Model * Create(Config& modelConfig);
	static Model * Create(Mesh * mesh);
	static void Destroy(Model * model);

	~Model();

	const ModelId& getId();
	const ModelName& getName();

	void setName(std::string newName);

	void initFromAi(const Resource::Assimp * assimpResource);
	void addMesh(Mesh * mesh);
	void addNode(Node * node);
	
	void removeMesh(Mesh * mesh);
	void removeNode(Node * node);

	void setScale(vec3 scale);
	void rotate(vec3 rotate, float angle);
	void setPosition(vec3 position);
	void setPosition(float x, float y, float z);
	void setQuaternion(float x, float y, float z, float w);

	const ModelMeshes& getMeshes();
	Mesh * getFirstMesh();

	const int getNodesCount();

	void setCurrentAnimation(std::string name);
	void tickAnimationTime(float time);
	void processAnimation();
	bool isAnimationProgress();
private:
	Model(Config& modelConfig);
	Model(Mesh * mesh);

	void destroy();

	void allocMeshes(unsigned int);
	void freeMeshes();
	void freeNodes();

	void createSkeleton(const Resource::Assimp * assimpResource, RawSkeleton * skeleton);
	void createJoints(const aiNode * node, RawSkeleton::Joint * joint);

	ModelId id_;
	ModelName name_;
	ModelMeshes meshes_;
	Node * rootNode_ = nullptr;
	Object3D object;
	std::unordered_map<std::string, std::shared_ptr<const Node>> nodes_;
	std::unordered_map<std::string, ImageLoader::Data> images_;

	ozz::animation::Skeleton * skeleton;
	animation::Animation * currentAnimation;
	std::unordered_map<std::string, animation::Animation*> animations_;
	std::map<Mesh*, std::vector<BoneMap>> boneMeshMap;
};

#endif
