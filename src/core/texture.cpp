#include "texture.h"

Texture::Texture(Texture_Type type)
	: type(type)
	, name(std::string(Texture::nameByType(type)))
{}

Texture::Texture(aiTextureType type)
	: type(getType(type))
	, name("")
{}

Texture::Texture(const Texture& other)
	: type(other.type)
	, name(other.name)
	, gTexture(other.gTexture)
	, images_(other.images_) // @todo: проверить удаление ресурсов. Потенциальная утечка памяти.
{}

Texture::~Texture() {}

Texture Texture::Empty(Texture_Type type = Texture_Diffuse, unsigned char color = 0)
{
	ImageLoader::RawData * image = new ImageLoader::RawData[3]{color, color, color};
	ImageLoader::Data imageData(image, 1, 1, GL_RGB);

	Texture texture(type);
	texture.setData(imageData);

	return texture;
}

Texture Texture::White(Texture_Type type)
{
	return Texture::Empty(type, 255);
}

Texture Texture::Cube()
{
	Texture texture(Texture_Cube);

	return texture;
}

Texture Texture::loadFromMaterial(aiMaterial * mat, aiTextureType type, const Resource::Assimp * assimpResource)
{
	unsigned int countTextures = mat->GetTextureCount(type);

	Texture texture(type);

	if (countTextures > 0) {
		std::string path = assimpResource->getTexturePath(mat, type, 0);

		ImageLoader::Data imageData = ImageLoader::load(path);

		if (!imageData.isReady()) {
			imageData = ImageLoader::load(assimpResource->getDefaultTexturePath());
		}

		texture.setData(imageData);
	} else {
//		console::warnp("texture %s not exist, create empty", Texture::charType(type));
		Texture_Type textureType = Texture::getType(type);
		switch (textureType) {
		case Texture_Specular:
			texture = Texture::Empty(textureType, 0); break;
		default:
			texture = Texture::White(textureType);
		}
	}

	return texture;
}

#ifdef GRAPHIC_API_OPENGL
void Texture::initTexture()
{
	// boost::mutex::scoped_lock lock(textureInitMutex);
}

// @todo: осторожно добавлять данные, могут потеряться уже существующие данные
void Texture::setData(ImageLoader::Data imageData)
{
	TextureImages::iterator it = images_.find(0);
	if (it != images_.end()) {
		images_.erase(it);
	}

	images_.insert(std::pair<char, ImageLoader::Data>(0, imageData));
}

void Texture::setData(ImageLoader::Data imageData, char index)
{
	TextureImages::iterator it = images_.find(index);
	if (it != images_.end()) {
		images_.erase(it);
	}

	images_.insert(std::pair<char, ImageLoader::Data>(index, imageData));
}

void Texture::setup()
{
	TextureImages::const_iterator got = images_.find(0);

	if (got == images_.end()) {
		console::warn("texture images map is empty", name);
		return;
	}

	switch (type) {
	case Texture_Diffuse:
	case Texture_Specular:
	case Texture_Height:
	case Texture_Normal:
		gTexture.setup2d(got->second); break;
	case Texture_Cube:
		gTexture.setupCube(images_); break;
	}
}

void Texture::destroy()
{
	gTexture.destroy();
	console::info("destroy texture");
	// imageData_->free();
	// delete imageData_;// @todo: not safe data remove

	TextureImages::iterator it;

	for (it = images_.begin(); it != images_.end(); it++) {
		ImageLoader::Data& imageData = it->second;

		imageData.free();
	}

	images_.clear();
}

#endif
