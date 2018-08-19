#include "model.h"
#include <algorithm>
#include <thread>
#include <mutex>
#include <queue>
#include <unordered_set>
#include "../lib/console.h"

// #define THREAD_INIT_AI_

Model::Node::Node() : name("") {}

Model::Node::~Node() {}

Model::Node::Node(aiNode *node) : name(std::string(node->mName.C_Str())) {
	meshes.reserve(node->mNumMeshes);
	children.reserve(node->mNumChildren);

	transformation = libAi::toNativeType(node->mTransformation);
}

void Model::Node::add(Mesh *mesh) {
	meshes.push_back(mesh);
}

void Model::Node::add(Model::Node *node) {
	children.push_back(node);
}

Model::Model() : name_() {}

Model::Model(Mesh *mesh) : Model() {
	add(mesh);
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

void Model::setName(const std::string &newName) {
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

void Model::add(Mesh *mesh) {
	meshes_.push_back(mesh);
}

void Model::add(Node *node) {
	nodes_.insert({node->name, std::shared_ptr<Node>(node)});
}

void Model::remove(Mesh *mesh) {
	auto it = std::find_if(meshes_.begin(), meshes_.end(), [&mesh](const Mesh *ptr) {
		return ptr == mesh;
	});

	if (it != meshes_.end()) {
		meshes_.erase(it);
	}
}

void Model::remove(Node *node) {
	auto it = nodes_.find(node->name);

	if (it != nodes_.end()) {
		nodes_.erase(it);
	}
}

const ModelMeshes &Model::getMeshes() {
	return meshes_;
}

Mesh *Model::getMesh() {
	return meshes_.at(0);
}

const size_t Model::getNodesCount() {
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