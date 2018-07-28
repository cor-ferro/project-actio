#include "model.h"
#include <algorithm>
#include <thread>
#include <mutex>
#include <queue>
#include <unordered_set>
#include "model.h"
#include "../lib/console.h"
#include "../lib/ozz.h"
#include "../lib/assimp.h"
#include "../math/Box3.h"

// #define THREAD_INIT_AI_

Model::File::File()
		: name("")
		, flipUv(false) {}

Model::File::File(std::string filePath)
		: file(filePath) {}

Model::File::File(const Model::File &other)
		: name(other.name)
		, file(other.file)
		, flipUv(other.flipUv)
		, animation(other.animation) {}

Model::Node::Node() : name("") {}

Model::Node::~Node() {}

Model::Node::Node(aiNode *node) : name(std::string(node->mName.C_Str())) {
	meshes.reserve(node->mNumMeshes);
	children.reserve(node->mNumChildren);

	transformation = libAi::toNativeType(node->mTransformation);
}

void Model::Node::addMesh(Mesh *mesh) {
	meshes.push_back(mesh);
}

void Model::Node::addNode(Model::Node *node) {
	children.push_back(node);
}

Model::Model() : name_() {}

Model::Model(Mesh *mesh) : Model() {
	addMesh(mesh);
}

Model::Model(Model::File &modelFile) : Model() {
	setName(modelFile.name);
}

Model::~Model() {
	destroy();
}

void Model::destroy() {
	freeMeshes();
	freeNodes();
}

const ModelName &Model::getName() {
	return name_;
}

void Model::setName(std::string newName) {
	name_ = newName;
}

void Model::freeMeshes() {
	console::info("destroy model %s", name_);

	for (auto mesh : meshes_) {
		Mesh::Destroy(mesh);
	}

	meshes_.clear();
}

void Model::freeNodes() {
	nodes_.clear();
}

void Model::addMesh(Mesh *mesh) {
	meshes_.push_back(mesh);
}

void Model::addNode(Node *node) {
	nodes_.insert({node->name, std::shared_ptr<Node>(node)});
}

void Model::removeMesh(Mesh *mesh) {
	ModelMeshes::iterator it = std::find_if(meshes_.begin(), meshes_.end(), [&mesh](const Mesh *ptr) {
		return ptr == mesh;
	});

	if (it != meshes_.end()) {
		meshes_.erase(it);
	}
}

void Model::removeNode(Node *node) {
	auto it = nodes_.find(node->name);

	if (it != nodes_.end()) {
		nodes_.erase(it);
	}
}

const ModelMeshes &Model::getMeshes() {
	return meshes_;
}

Mesh *Model::getFirstMesh() {
	return meshes_.at(0);
}

Mesh *Model::getMesh() {
	return meshes_.at(0);
}

const int Model::getNodesCount() {
	return nodes_.size();
}

float Model::height() {
	float maxHeight = 0.0f;

	for (auto mesh : meshes_) {
		float geometryHeight = mesh->geometry.height();

		if (geometryHeight > maxHeight) {
			maxHeight = geometryHeight;
		}
	}

	return maxHeight;
}

float Model::halfHeight() {
	float maxHeight = height();

	if (maxHeight > 0.0f) {
		return maxHeight / 2.0f;
	}

	return 0.0f;
}

Math::Box3 Model::getBoundingBox() {
	Math::Box3 maxBox;

	for (auto mesh : meshes_) {
		const Math::Box3 &bBox = mesh->geometry.getBoundingBox();

		maxBox.radius.x = glm::max(bBox.radius.x, maxBox.radius.x);
		maxBox.radius.y = glm::max(bBox.radius.y, maxBox.radius.y);
		maxBox.radius.z = glm::max(bBox.radius.z, maxBox.radius.z);
	}

	return maxBox;
}

void Model::reindexMeshBones(std::unordered_map<std::string, uint> &nodeIndexes) {
	// fill mesh node->bone indexes
	for (auto &mesh : meshes_) {
		for (unsigned int boneId = 0; boneId < mesh->bones.count; boneId++) {
			auto nodeIndexIt = nodeIndexes.find(mesh->bones.names[boneId]);

			if (nodeIndexIt != nodeIndexes.end()) {
				mesh->bones.indexes[boneId] = nodeIndexIt->second;
			} else {
				console::warn("node not found: %s", mesh->bones.names[boneId]);
				mesh->bones.indexes[boneId] = 0;
			}
		}
	}
}