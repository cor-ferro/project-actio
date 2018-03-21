/*
 * assimp_resource.h
 *
 *  Created on: Oct 14, 2017
 *      Author: demitriy
 */

#ifndef ASSIMP_RESOURCE_H_
#define ASSIMP_RESOURCE_H_

#include <string>
#include <vector>
#include <unordered_set>
#include <assimp/scene.h>
#include <boost/filesystem/path.hpp>
#include "../app/app.h"
#include "../lib/path.h"

namespace Resource {
	struct Assimp {
		Assimp(const aiScene * scene, std::string path);
		~Assimp();

		std::string getTexturePath(const aiMaterial * mat, aiTextureType type, unsigned int index) const;
		std::string getDefaultTexturePath() const;

		std::unordered_set<std::string> getTexturePaths() const;
		std::vector<aiNode*> getAllNodes() const;

		const aiNode * getRootNode() const;
		const aiMesh * getMesh(uint index) const;
		const aiMaterial * getMeshMaterial(const aiMesh * mesh) const;

		aiScene const * scene;
		std::string resourcePath_;
		std::string basePath_;
	private:
		void collectNode(aiNode * node, std::vector<aiNode*>& nodes) const;
	};
}



#endif /* ASSIMP_RESOURCE_H_ */
