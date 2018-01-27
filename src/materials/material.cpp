#include "material.h"

namespace Material {
	Base::Base()
	{
		wireframe = false;
	}

	Base::Base(Texture texture) : wireframe(false)
	{
		addTexture(texture);
	}

	Base::~Base() {}

	void Base::addTexture(Texture texture)
	{
		textures_.push_back(texture);
	}

	void Base::setupTextures()
	{
		for (Texture& texture : textures_)
		{
			texture.setup();
		}
	}

	void Base::setWireframe(bool flag)
	{
		wireframe = flag;
	}

	const MaterialTextures& Base::getTextures()
	{
		return textures_;
	}

	Phong::Phong(Texture texture)
		: Base(texture)
		, shininess(1.0f)
		, noLight(false)
	{}

	Phong::Phong()
		: Base()
		, shininess(1.0f)
		, noLight(false)
	{}

	Phong::~Phong() {};

	void Phong::initFromAi(const aiMaterial * material, const Resource::Assimp * assimpResource, std::unordered_map<std::string, ImageLoader::Data>& images)
	{
		if (!material) {
			initEmptyTextures();
			return;
		}

		textures_.clear();
		textures_.reserve(4);

		Phong meshMaterial;
		Texture diffuseMap = Texture::Empty(Texture_Diffuse, 155);
		Texture specularMap = Texture::Empty(Texture_Specular, 155);
		Texture heightMap = Texture::Empty(Texture_Height, 155);
		Texture normalMap = Texture::Empty(Texture_Normal, 155);

		float shininessValue = 0.0f;
		float reflectivity = 0.0f;
		float refracti = 0.0f;
		float colorReflective = 0.0f;
		bool isWireframe = false;

		aiColor3D diffuseColor (0.f, 0.f, 0.f);
		aiColor3D specularColor (0.f, 0.f, 0.f);
		aiColor3D ambientColor (0.f, 0.f, 0.f);
		material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
		material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
		material->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor);

		material->Get(AI_MATKEY_SHININESS, shininessValue);
		material->Get(AI_MATKEY_ENABLE_WIREFRAME, wireframe);

		setDiffuse(diffuseColor[0], diffuseColor[1], diffuseColor[2]);
		setSpecular(specularColor[0], specularColor[1], specularColor[2]);
		setAmbient(ambientColor[0], ambientColor[1], ambientColor[2]);
		setWireframe(isWireframe);
		setShininess(shininessValue);

		unsigned int countDiffuseTextures = material->GetTextureCount(aiTextureType_DIFFUSE);
		unsigned int countSpecularTextures = material->GetTextureCount(aiTextureType_SPECULAR);
		unsigned int countHeightTextures = material->GetTextureCount(aiTextureType_HEIGHT);
		unsigned int countNormalTextures = material->GetTextureCount(aiTextureType_NORMALS);

		if (countDiffuseTextures > 0) {
			std::string texturePath = assimpResource->getTexturePath(material, aiTextureType_DIFFUSE, 0);
			std::unordered_map<std::string, ImageLoader::Data>::iterator it = images.find(texturePath);

			if (it != images.end()) diffuseMap.setData(it->second);
		}

		if (countSpecularTextures > 0) {
			std::string texturePath = assimpResource->getTexturePath(material, aiTextureType_SPECULAR, 0);
			std::unordered_map<std::string, ImageLoader::Data>::iterator it = images.find(texturePath);

			if (it != images.end()) specularMap.setData(it->second);
		}

		if (countHeightTextures > 0) {
			std::string texturePath = assimpResource->getTexturePath(material, aiTextureType_HEIGHT, 0);
			std::unordered_map<std::string, ImageLoader::Data>::iterator it = images.find(texturePath);

			if (it != images.end()) heightMap.setData(it->second);
		}

		if (countNormalTextures > 0) {
			std::string texturePath = assimpResource->getTexturePath(material, aiTextureType_NORMALS, 0);
			std::unordered_map<std::string, ImageLoader::Data>::iterator it = images.find(texturePath);

			if (it != images.end()) normalMap.setData(it->second);
		}

		textures_.push_back(diffuseMap);
		textures_.push_back(specularMap);
		textures_.push_back(heightMap);
		textures_.push_back(normalMap);
	}

	void Phong::initEmptyTextures()
	{
		Texture diffuseMap  = Texture::White(Texture_Diffuse);
		Texture specularMap = Texture::Empty(Texture_Specular, 1);
		Texture heightMap   = Texture::White(Texture_Height);
		Texture normalMap   = Texture::White(Texture_Normal);

		textures_.clear();
		textures_.reserve(4);
		textures_.push_back(diffuseMap);
		textures_.push_back(specularMap);
		textures_.push_back(heightMap);
		textures_.push_back(normalMap);
	}

	void Phong::setAmbient(vec3 ambient) { this->ambient = ambient; }
	void Phong::setAmbient(float r, float g, float b)
	{
		ambient[0] = r;
		ambient[1] = g;
		ambient[2] = b;
	}

	void Phong::setDiffuse(vec3 diffuse) { this->diffuse = diffuse; }
	void Phong::setDiffuse(float r, float g, float b)
	{
		diffuse[0] = r;
		diffuse[1] = g;
		diffuse[2] = b;
	}

	void Phong::setSpecular(vec3 specular) { this->specular = specular; }
	void Phong::setSpecular(float r, float g, float b)
	{
		specular[0] = r;
		specular[1] = g;
		specular[2] = b;
	}

	void Phong::setColor(vec3 color)
	{
		setAmbient(color);
		setDiffuse(color);
		setSpecular(color);
	}

	void Phong::setColor(float r, float g, float b)
	{
		setAmbient(r, g, b);
		setDiffuse(r, g, b);
		setSpecular(r, g, b);
	}

	void Phong::setShininess(float shininess)
	{
		this->shininess = shininess;
	}

	void Phong::setNoLight(bool isNoLight)
	{
		noLight = isNoLight;
	}

	bool Phong::isNoLight()
	{
		return noLight;
	}
}
