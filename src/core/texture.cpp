#include "texture.h"

Texture::Texture() {}
Texture::Texture(const Texture& other)
{
	type = other.type;
	name = other.name;
	textureID = other.textureID;
	textureTarget = other.textureTarget;
	images_ = other.images_; // @todo: проверить удаление ресурсов. Потенциальная утечка памяти.
}

Texture::~Texture()
{
	// console::info("destory texture");
	// freeData();
}

Texture Texture::White()
{
	ImageLoader::RawData * image = new ImageLoader::RawData[1]{255};
	ImageLoader::Data imageData(image, 1, 1);
	imageData.format = GL_RGB;

	Texture texture;
	texture.setAsDiffuse();
	texture.setData(imageData);

	return texture;
}

Texture Texture::Cube()
{
	Texture texture;
	texture.setAsCube();

	return texture;
}

Texture Texture::loadFromMaterial(aiMaterial * mat, aiTextureType type, const Resource::Assimp * assimpResource)
{
	unsigned int countTextures = mat->GetTextureCount(type);

	Texture texture;

	if (countTextures > 0) {
		std::string path = assimpResource->getTexturePath(mat, type, 0);
		ImageLoader::Data imageData = ImageLoader::load(path);

		if (!imageData.isReady()) {
			console::warn("failed load image", path.c_str());
			imageData = ImageLoader::load(assimpResource->getDefaultTexturePath());
		}

		texture.setData(imageData);
	} else {
		// console::info("texture", type, "not exist, create empty");
		texture = Texture::White();
	}

	switch (type) {
	case aiTextureType_DIFFUSE:
		texture.setAsDiffuse();
		break;
	case aiTextureType_SPECULAR:
		texture.setAsSpecular();
		break;
	case aiTextureType_HEIGHT:
		texture.setAsHeight();
		break;
	case aiTextureType_NORMALS:
		texture.setAsNormal();
		break;
	default:
		texture.setAsDiffuse();
		console::err("unknown texture type");
	}

	return texture;
}

#ifdef GRAPHIC_API_OPENGL
void Texture::initTexture()
{
	// boost::mutex::scoped_lock lock(textureInitMutex);
}

void Texture::setData(ImageLoader::Data imageData)
{
	images_.insert(std::pair<char, ImageLoader::Data>(0, imageData));
}

void Texture::setData(ImageLoader::Data imageData, char index)
{
	images_.insert(std::pair<char, ImageLoader::Data>(index, imageData));
}

void Texture::setup2d()
{
	boost::mutex::scoped_lock lock(textureLoaderMutex);

	TextureImages::const_iterator got = images_.find(0);

	if (got == images_.end()) {
		console::warn("texture images map is empty", name);
		return;
	}

	ImageLoader::Data imageData = got->second;

	// @todo проверять данные
	// if (!imageData_) {
	// 	console::warn("try bind empty data in texture");
	// 	return;
	// }

	textureTarget = GL_TEXTURE_2D;

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageData.width, imageData.height, 0, imageData.format, GL_UNSIGNED_BYTE, imageData.get());
	
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::setupCube()
{
	boost::mutex::scoped_lock lock(textureLoaderMutex);
	// if (!imageData_) {
	// 	console::warn("try bind empty data in texture");
	// 	return;
	// }

	textureTarget = GL_TEXTURE_CUBE_MAP;

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	for (int i = 0; i < 6; i++) {
		TextureImages::const_iterator got = images_.find(i);

		if (got == images_.end()) {
			console::warn("cannot get image map", i);
			continue;
		}

		ImageLoader::Data imageData = got->second;

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, imageData.width, imageData.height, 0, imageData.format, GL_UNSIGNED_BYTE, imageData.get());
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);  
}

void Texture::unbindData()
{
	// glDeleteTextures(1, &textureID);
}

void Texture::freeData()
{
	console::info("free data");
	// imageData_->free();
	// delete imageData_;// @todo: not save data remove
	// unbindData();
}

void Texture::setAsDiffuse()
{
	type = Texture_Diffuse;
	name = "diffuseTexture";
}

void Texture::setAsSpecular()
{
	type = Texture_Specular;
	name = "specularTexture";
}

void Texture::setAsHeight()
{
	type = Texture_Height;
	name = "heightTexture";
}

void Texture::setAsNormal()
{
	type = Texture_Normal;
	name = "normalTexture";
}

void Texture::setAsCube()
{
	type = Texture_Cube;
	name = "cubeTexture";
}


#endif
