#ifndef ACTIO_CORE_MODEL_H_
#define ACTIO_CORE_MODEL_H_

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
#include "mesh.h"
#include "../lib/image_loader.h"
#include "../lib/comparators.h"
#include "../resources/resources.h"
#include "../animation/animation.h"
#include "../animation/process.h"

#include "../memory/poolallocator.h"

using ozz::animation::offline::RawSkeleton;

extern memory::PoolAllocator * modelsAllocator;

typedef std::string ModelName;
typedef std::vector<Mesh*> ModelMeshes;

typedef std::string ModelName;
typedef std::vector<Mesh *> ModelMeshes;

class ModelBuilder;

struct Model {
	friend class ModelBuilder;

	struct File {
		File();

		File(std::string filePath);

		File(const File &other);

		std::string name;
		Resource::File file;
		bool flipUv;
		std::string animation;
	};

	struct Node {
		Node();

		explicit Node(aiNode *node);

		~Node();

		void add(Mesh *mesh);

		void add(Node *node);

		std::string name;
		std::vector<Node *> children;
		std::vector<Mesh *> meshes;
		mat4 transformation;
	};

	Model();

	explicit Model(File &modelFile);

	explicit Model(Mesh *mesh);

	~Model();

	const ModelName &getName();

	void setName(const std::string &newName);

	void add(Mesh *mesh);

	void add(Node *node);

	void remove(Mesh *mesh);

	void remove(Node *node);

	const ModelMeshes &getMeshes();

	Mesh *getMesh();

	const int getNodesCount();

	float height();

	float halfHeight();

	Math::Box3 getBoundingBox();

	void reindexMeshBones(std::unordered_map<std::string, uint> &nodeIndexes);

private:
	void destroy();

	void freeMeshes();

	void freeNodes();

	ModelName name_;
	ModelMeshes meshes_;
	Node *rootNode_ = nullptr;
	std::unordered_map<std::string, std::shared_ptr<const Node>> nodes_;
};

#endif
