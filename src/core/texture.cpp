#include "texture.h"

Texture::~Texture()
{
//	freeData();
}

Texture Texture::White()
{
	unsigned char * image = new unsigned char[1]{255};
	
	Loader::ImageData * imageData = new Loader::ImageData();
	imageData->set(image, 1);
	imageData->calcSize();
	imageData->format = GL_RGB;

	Texture texture;
	texture.initTexture();
	texture.setData(imageData);
	// texture.bindData(imageData);

	return texture;
}

std::vector<Texture> Texture::loadFromMaterial(aiMaterial * mat, aiTextureType type, const Resource::Assimp * assimpResource)
{
	unsigned int countTextures = mat->GetTextureCount(type);
	std::vector<Texture> textures;
	textures.reserve(countTextures);

	for (unsigned int i = 0; i < countTextures; i++)
	{
		Texture texture;
		// texture.initTexture();

		boost::filesystem::path texturePath(assimpResource->getTexturePath(mat, type, i));
		
		std::string p = "resources/lina/";
		std::string fixedTexturePath = texturePath.string();
		std::replace(fixedTexturePath.begin(), fixedTexturePath.end(), '\\', '/');
		// fixedTexturePath.replace(0, 71, ""); // lina
		// fixedTexturePath.replace(0, 75, ""); // antimage
		// fixedTexturePath.replace(0, 72, ""); // lich king
		// fixedTexturePath = p + fixedTexturePath;
		
		// std::string path = assimpResource->getTexturePath(mat, type, i);
		std::string path = fixedTexturePath;
		// console::info("texture", path);

		Loader::ImageData * imageData = Loader::Image(path.c_str());

		// non tread safe
		if (!imageData->isReady()) {
			console::warn("failed load image", path.c_str());
			delete imageData;
			imageData = Loader::Image(assimpResource->getDefaultTexturePath().c_str());
		}
		
		texture.setData(imageData);
		// texture.bindData();

		switch (type) {
		case aiTextureType_DIFFUSE:
			texture.type = "diffuseTexture";
			break;
		case aiTextureType_SPECULAR:
			texture.type = "specularTexture";
			break;
		case aiTextureType_HEIGHT:
			texture.type = "heightTexture";
			break;
		case aiTextureType_NORMALS:
			texture.type = "normalTexture";
			break;
		default:
			console::err("unknown texture type");
		}

		textures.push_back(texture);
	}

	return textures;
}

#ifdef GRAPHIC_API_OPENGL
void Texture::initTexture()
{
	
}

void Texture::setData(Loader::ImageData * imageData)
{
	imageData_ = imageData;
}

void Texture::bindData()
{
	boost::mutex::scoped_lock lock(textureLoaderMutex);

	if (imageData_ == nullptr) {
		console::warn("try bind empty data in texture");
		return;
	}

	GLuint texId;
	glGenTextures(1, &texId);
	textureID = texId;

	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageData_->width, imageData_->height, 0, imageData_->format, GL_UNSIGNED_BYTE, imageData_->get());
	
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::unbindData()
{
	// glDeleteTextures(1, &textureID);
}

void Texture::freeData()
{
	console::info("unbindData");
	// imageData_->free();
	// delete imageData_;// @todo: not save data remove
	// unbindData();
}

#endif
