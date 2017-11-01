/*
 * scene_context_loader.h
 *
 *  Created on: Aug 28, 2017
 *      Author: demitriy
 */

#ifndef SCENE_CONTEXT_LOADER_H_
#define SCENE_CONTEXT_LOADER_H_

#include <assimp/scene.h>
#include <boost/filesystem/path.hpp>

struct SceneContextLoader {
	void setScene(const aiScene * scene);
	void setBaseFilePath(std::string);

	aiScene * scene;
	boost::filesystem::path baseFilePath;
};



#endif /* SCENE_CONTEXT_LOADER_H_ */
