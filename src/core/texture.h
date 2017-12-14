#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <glad/glad.h>

#include <memory>
#include <iostream>
#include <vector>
#include <string>
#include <boost/thread.hpp>
#include <assimp/scene.h>
#include <unordered_map>
#include "../lib/image_loader.h"
#include "../app/app.h"
#include "../lib/console.h"
#include "../resources/resources.h"
#ifdef GRAPHIC_API_OPENGL

#endif

#ifdef GRAPHIC_API_OPENGL
typedef GLuint TextureId;
#endif

static boost::mutex textureInitMutex;
static boost::mutex textureLoaderMutex;

typedef std::unordered_map<char, ImageLoader::Data> TextureImages;

enum Texture_Type {
	Texture_Diffuse = 1,
	Texture_Specular = 2,
	Texture_Height = 3,
	Texture_Normal = 4,
	Texture_Cube = 5
};

struct Texture {
	Texture();
	Texture(const Texture& texture);
	~Texture();
	static Texture White();
	static Texture Cube();
	static Texture loadFromMaterial(aiMaterial *, aiTextureType, const Resource::Assimp * assimpResource);

	void initTexture();
	void setup2d();
	void setupCube();
	void unbindData();
	void setData(ImageLoader::Data imageData);
	void setData(ImageLoader::Data imageData, char);
	void freeData();

	void setAsDiffuse();
	void setAsSpecular();
	void setAsHeight();
	void setAsNormal();
	void setAsCube();

	Texture_Type type;
	std::string name;

	#ifdef GRAPHIC_API_OPENGL
	TextureId textureID;
	GLenum textureTarget;
	#endif
private:
	TextureImages images_;
};

#endif
