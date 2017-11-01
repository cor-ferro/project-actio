/*
 * assimp_resource.h
 *
 *  Created on: Oct 14, 2017
 *      Author: demitriy
 */

#ifndef ASSIMP_RESOURCE_H_
#define ASSIMP_RESOURCE_H_

#include <string>
#include <assimp/scene.h>
#include <boost/filesystem/path.hpp>
#include "../app/app.h"

namespace Resource {
	struct Assimp {
		Assimp(const aiScene * scene, std::string path);
		~Assimp();

		std::string getTexturePath(aiMaterial * mat, aiTextureType type, unsigned int index) const;
		std::string getDefaultTexturePath() const;

		aiScene const * scene;
		std::string resourcePath_;
		std::string basePath_;
	private:
	};
}



#endif /* ASSIMP_RESOURCE_H_ */
