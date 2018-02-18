#ifndef MODEL_H_
#define MODEL_H_

#include <memory>
#include <boost/thread.hpp>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <algorithm>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../lib/console.h"
#include "../lib/types.h"
#include "../lib/assimp.h"
#include "../lib/image_loader.h"
#include "../resources/resources.h"
#include "../animation/animation.h"
#include "../math/Box3.h"
#include "./object3D.h"
#include "./mesh.h"

#include "../memory/poolallocator.h"

#include "ozz/animation/runtime/animation.h"
#include "ozz/animation/runtime/local_to_model_job.h"
#include "ozz/animation/runtime/sampling_job.h"
#include "ozz/animation/runtime/skeleton.h"

#include "ozz/animation/offline/animation_builder.h"
#include "ozz/animation/offline/raw_animation.h"
#include "ozz/animation/offline/raw_skeleton.h"
#include "ozz/animation/offline/skeleton_builder.h"

#include "ozz/base/maths/quaternion.h"
#include "ozz/base/maths/simd_math.h"
#include "ozz/base/maths/soa_transform.h"
#include "ozz/base/maths/vec_float.h"

#include "ozz/base/memory/allocator.h"

using ozz::math::Float3;
using ozz::math::Float4;
using ozz::math::Quaternion;
using ozz::math::SoaTransform;
using ozz::math::Float4x4;
using ozz::animation::offline::RawSkeleton;
using ozz::animation::offline::RawAnimation;

extern memory::PoolAllocator * modelsAllocator;

typedef std::size_t ModelId;
typedef std::string ModelName;
typedef Mesh * ModelMesh;
typedef std::vector<ModelMesh> ModelMeshes;

static size_t idCounter = 0;
static ModelId newId()
{
	return ++idCounter;
}

struct ModelNode {
	ModelNode();
	ModelNode(aiNode * node);
	~ModelNode();

	void addMesh(Mesh * mesh);
	void addNode(ModelNode * node);

	std::string name;
	std::vector<std::shared_ptr<ModelNode>> children;
	std::vector<Mesh*> meshes;
	mat4 transformation;
};

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

	Model();

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
	void addNode(ModelNode * node);
	
	void removeMesh(Mesh * mesh);
	void removeNode(ModelNode * node);

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

	ozz::Range<ozz::math::Float4x4> * getBones();
private:
	Model(Config& modelConfig);
	Model(Mesh * mesh);
	Model(const Model& model);

	void destroy();

	void allocMeshes(unsigned int);
	void freeMeshes();
	void freeNodes();

	void createSkeleton(const Resource::Assimp * assimpResource, RawSkeleton * skeleton);
	void createJoints(const aiNode * node, RawSkeleton::Joint * joint);
	void buildSkeleton();

	void createAnimation(aiAnimation * assimpAnimation, ozz::animation::offline::RawAnimation * rawAnimation);

	ModelId id_;
	ModelName name_;
	ModelMeshes meshes_;
	ModelNode * rootNode_;
	Object3D object;
	bool animInterpolation_;
	std::unordered_map<std::string, std::shared_ptr<const ModelNode>> nodes_;
	std::unordered_map<std::string, std::shared_ptr<const Animation>> animations_;
	std::unordered_map<std::string, ImageLoader::Data> images_;
	ozz::animation::Skeleton * skeleton;
	ozz::animation::Animation * currentAnimation;
	float animationTime = 0.0f;
	std::vector<ozz::animation::offline::RawAnimation*> rawAnimations;
	std::unordered_map<std::string, ozz::animation::Animation*> animations;

	ozz::Range<ozz::math::SoaTransform> locals_;
	ozz::Range<ozz::math::Float4x4> bones_;
	ozz::animation::SamplingCache * cache_;
};

static std::vector<Model*> _models;

#endif
