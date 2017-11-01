#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <iostream>
#include <vector>
#include <string>
#include <boost/thread.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/regex.hpp>
#include <assimp/scene.h>
#include "../lib/image_loader.h"
#include "../app/app.h"
#include "../lib/console.h"
#include "../resources/resources.h"
#ifdef GRAPHIC_API_OPENGL
    #include <GL/glew.h>
    #include <GL/gl.h>
#endif

static boost::mutex textureLoaderMutex;

struct Texture {
	~Texture();
	static Texture White();
	static std::vector<Texture> loadFromMaterial(aiMaterial *, aiTextureType, const Resource::Assimp * assimpResource);

	void initTexture();
	void bindData();
	void unbindData();
	void setData(Loader::ImageData * imageData);

	std::string type;

	#ifdef GRAPHIC_API_OPENGL
	unsigned int textureID;
	void freeData();
	#endif
private:
	Loader::ImageData * imageData_;
};

#endif
