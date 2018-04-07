#include "assimp_resource.h"

namespace Resource {
	Assimp::Assimp(const aiScene * scene, std::string path) : scene(scene) {
		Path basePath(path);

		resourcePath_ = path;
		basePath_ = basePath.parent_path().string();

		console::info("resourcePath %s", resourcePath_);
		console::info("basePath %s", basePath_);
	}

	Assimp::~Assimp() {

	}

	std::string Assimp::getTexturePath(const aiMaterial * mat, aiTextureType type, unsigned int index) const {
		aiString tex;
		mat->GetTexture(type, index, &tex);
		return std::string(basePath_ + "/" + tex.C_Str());
	}

	std::string Assimp::getDefaultTexturePath() const {
		Path p = App::instance().resourcePath("default2.png");
		return p.string();
	}

	std::unordered_set<std::string> Assimp::getTexturePaths() const {
		std::unordered_set<std::string> texturePaths;

		const uint countMeshes = scene->mNumMeshes;

		for (uint i = 0; i < countMeshes; i++) {
			aiMesh * mesh = scene->mMeshes[i];

			assert(mesh != nullptr);

			if (mesh->mMaterialIndex < 0) continue;

			aiMaterial * material = scene->mMaterials[mesh->mMaterialIndex];

			if (material == nullptr) continue;

			unsigned int countDiffuseTextures = material->GetTextureCount(aiTextureType_DIFFUSE);
			unsigned int countSpecularTextures = material->GetTextureCount(aiTextureType_SPECULAR);
			unsigned int countHeightTextures = material->GetTextureCount(aiTextureType_HEIGHT);
			unsigned int countNormalTextures = material->GetTextureCount(aiTextureType_NORMALS);

			if (countDiffuseTextures > 0) {
				std::string path = getTexturePath(material, aiTextureType_DIFFUSE, 0);
				texturePaths.insert(path);
			}

			if (countSpecularTextures > 0) {
				std::string path = getTexturePath(material, aiTextureType_DIFFUSE, 0);
				texturePaths.insert(path);
			}

			if (countHeightTextures > 0) {
				std::string path = getTexturePath(material, aiTextureType_DIFFUSE, 0);
				texturePaths.insert(path);
			}

			if (countNormalTextures > 0) {
				std::string path = getTexturePath(material, aiTextureType_DIFFUSE, 0);
				texturePaths.insert(path);
			}
		}

		return texturePaths;
	}

	std::vector<aiNode*> Assimp::getAllNodes() const {
		std::vector<aiNode*> nodes;

		collectNode(scene->mRootNode, nodes);

		return nodes;
	}

	void Assimp::collectNode(aiNode * node, std::vector<aiNode*>& nodes) const {
		nodes.push_back(node);

		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			aiNode * childNode = node->mChildren[i];
			collectNode(childNode, nodes);
		}
	}

	aiScene const * Assimp::getScene() const {
		return scene;
	}

	const aiNode * Assimp::getRootNode() const {
		return scene->mRootNode;
	}

	const aiMesh * Assimp::getMesh(uint index) const {
		if (scene->mMeshes[index]) {
			return scene->mMeshes[index];
		}

		return nullptr;
	}

	const aiMaterial * Assimp::getMeshMaterial(const aiMesh * mesh) const {
		if (scene->mMaterials[mesh->mMaterialIndex]) {
			return scene->mMaterials[mesh->mMaterialIndex];
		}

		return nullptr;
	}

	bool Assimp::hasAnimations() const {
	    return scene->mNumAnimations > 0;
	}
}
