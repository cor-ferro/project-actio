#include "assimp_resource.h"

namespace Resource {
	Assimp::Assimp(const aiScene * scene, std::string path) : scene(scene) {
		boost::filesystem::path boostBasePath(path);

		resourcePath_ = path;
		basePath_ = boostBasePath.parent_path().string();

		console::info("resourcePath_", resourcePath_);
		console::info("basePath_", basePath_);
	}

	Assimp::~Assimp() {

	}

	std::string Assimp::getTexturePath(aiMaterial * mat, aiTextureType type, unsigned int index) const {
		aiString tex;
		mat->GetTexture(type, index, &tex);

		std::string path = basePath_ + "/" + tex.C_Str();

		return path;
	}

	std::string Assimp::getDefaultTexturePath() const {
		return App::instance().resourcePath("white_hills.jpg");
	}
}
