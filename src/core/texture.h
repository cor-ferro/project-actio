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
#include "../renderer/opengl/texture.h"
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
	Texture_Cube = 5,
	Texture_Empty = 6
};

struct Texture {
	Texture(Texture_Type type);
	Texture(aiTextureType type);
	Texture(const Texture& texture);
	~Texture();
	static Texture Empty(Texture_Type type, unsigned char color);
	static Texture White(Texture_Type type);
	static Texture Cube();
	static Texture loadFromMaterial(aiMaterial *, aiTextureType, const Resource::Assimp * assimpResource);
	static Texture_Type getType(aiTextureType type) {
		switch (type) {
		case aiTextureType_DIFFUSE: return Texture_Diffuse;
		case aiTextureType_SPECULAR: return Texture_Specular;
		case aiTextureType_HEIGHT: return Texture_Height;
		case aiTextureType_NORMALS: return Texture_Normal;
		default: return Texture_Diffuse;
		}
	};
	static const char * charType(aiTextureType type) {
		switch (type) {
		case aiTextureType_DIFFUSE: return "diffuse"; break;
		case aiTextureType_SPECULAR: return "specular"; break;
		case aiTextureType_HEIGHT: return "height"; break;
		case aiTextureType_NORMALS: return "normal"; break;
		default: return "diffuse";
		}
	};
	static const char * nameByType(Texture_Type type) {
		switch (type) {
			case Texture_Diffuse: return "diffuseTexture"; break;
			case Texture_Specular: return "specularTexture"; break;
			case Texture_Height: return "heightTexture"; break;
			case Texture_Normal: return "normalTexture"; break;
			case Texture_Cube: return "cubeTexture"; break;
			default: return "diffuseTexture";
		}
	}

	void initTexture();
	void setup();
	void unbindData();
	void setData(ImageLoader::Data imageData);
	void setData(ImageLoader::Data imageData, char);
	void destroy();

	Texture_Type type;
	std::string name;

	OpenglTexture gTexture;
private:
	TextureImages images_;
};

#endif
