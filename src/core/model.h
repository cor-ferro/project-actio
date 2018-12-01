#ifndef ACTIO_CORE_MODEL_H_
#define ACTIO_CORE_MODEL_H_

#include <memory>
#include <unordered_map>
#include <map>
#include <vector>
#include <memory>
#include "model.h"
#include "mesh.h"
#include "../lib/image_loader.h"
#include "../lib/comparators.h"
#include "../resources/resources.h"
#include "../math/Box3.h"

#include "../memory/poolallocator.h"

typedef std::string ModelName;
typedef std::vector<std::shared_ptr<Mesh>> ModelMeshes;

class ModelBuilder;

struct Model {
	friend class ModelBuilder;

	struct Node {
		Node();

		explicit Node(aiNode *node);

		~Node();

		void add(MeshHandle &mesh);

		void add(Node *node);

		std::string name;
		std::vector<Node *> children;
		std::vector<MeshHandle> meshes;
		mat4 transformation;
	};

	Model();

	explicit Model(MeshHandle &mesh);

	~Model();

	const ModelName &getName();

	void setName(const std::string &newName);

	void add(MeshHandle &mesh);

	void add(Node *node);

	void remove(Node *node);

	const ModelMeshes &getMeshes();

    MeshHandle &getMesh();

	const size_t getNodesCount();

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
