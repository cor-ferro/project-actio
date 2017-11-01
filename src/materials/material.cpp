#include "material.h"

Material::Material()
{
	wireframe = false;
}

Material::~Material()
{

}

void Material::texturesBindData()
{
	for (auto &texture : textures_)
	{
		texture.bindData();
	}
}

void Material::setWireframe(bool flag)
{
	wireframe = flag;
}

const std::vector<Texture>& Material::getTextures()
{
	return textures_;
}

PhongMaterial::PhongMaterial()
{
	shininess = 30.0f;
};

PhongMaterial::~PhongMaterial() {};

PhongMaterial::PhongMaterial(aiMaterial * material, const Resource::Assimp * assimpResource)
{
	shininess = 30.0f;
	initFromAi(material, assimpResource);
}

void PhongMaterial::initFromAi(aiMaterial * material, const Resource::Assimp * assimpResource)
{
	if (!material) {
		return;
	}

	aiColor3D diffuseColor(0.0f, 0.0f, 0.0f);
	aiColor3D specularColor(0.0f, 0.0f, 0.0f);
	aiColor3D ambientColor(0.0f, 0.0f, 0.0f);
	float shininessValue = 0.0;
	bool isWireframe = false;

	// @todo: uncomment get material color and fix shader
	//material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
	//material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
	//material->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor);
	material->Get(AI_MATKEY_SHININESS, shininessValue);
	// material->Get(AI_MATKEY_WIREFRAME, wireframe); error: AI_MATKEY_WIREFRAME was not declared in this scope

	PhongMaterial meshMaterial;

	setDiffuse(diffuseColor.r, diffuseColor.g, diffuseColor.b);
	setSpecular(specularColor.r, specularColor.g, specularColor.b);
	setAmbient(ambientColor.r, ambientColor.g, ambientColor.b);
	setWireframe(isWireframe);
	setShininess(shininessValue);

	std::vector<Texture> diffuseMaps = Texture::loadFromMaterial(material, aiTextureType_DIFFUSE, assimpResource);
	std::vector<Texture> specularMaps = Texture::loadFromMaterial(material, aiTextureType_SPECULAR, assimpResource);
	std::vector<Texture> heightMaps = Texture::loadFromMaterial(material, aiTextureType_HEIGHT, assimpResource);
	std::vector<Texture> normalMaps = Texture::loadFromMaterial(material, aiTextureType_NORMALS, assimpResource);

	unsigned int countTextures = diffuseMaps.size() + specularMaps.size() + heightMaps.size();

	textures_.clear();
	textures_ = std::vector<Texture>();
	textures_.reserve(countTextures);

	textures_.insert(textures_.end(), diffuseMaps.begin(), diffuseMaps.end());
	textures_.insert(textures_.end(), specularMaps.begin(), specularMaps.end());
	textures_.insert(textures_.end(), heightMaps.begin(), heightMaps.end());
}

void PhongMaterial::initEmptyTextures()
{
	std::vector<Texture> diffuseMaps { Texture::White() };
	std::vector<Texture> specularMaps { Texture::White() };
	std::vector<Texture> heightMaps { Texture::White() };
	std::vector<Texture> normalMaps { Texture::White() };

	unsigned int countTextures = diffuseMaps.size() + specularMaps.size() + heightMaps.size();

	textures_.clear();
	textures_ = std::vector<Texture>();
	textures_.reserve(countTextures);

	textures_.insert(textures_.end(), diffuseMaps.begin(), diffuseMaps.end());
	textures_.insert(textures_.end(), specularMaps.begin(), specularMaps.end());
	textures_.insert(textures_.end(), heightMaps.begin(), heightMaps.end());
}

void PhongMaterial::setAmbient(vec3 ambient) { this->ambient = ambient; }
void PhongMaterial::setAmbient(float r, float g, float b)
{
	ambient[0] = r;
	ambient[1] = g;
	ambient[2] = b;
}

void PhongMaterial::setDiffuse(vec3 diffuse) { this->diffuse = diffuse; }
void PhongMaterial::setDiffuse(float r, float g, float b)
{
	diffuse[0] = r;
	diffuse[1] = g;
	diffuse[2] = b;
}

void PhongMaterial::setSpecular(vec3 specular) { this->specular = specular; }
void PhongMaterial::setSpecular(float r, float g, float b)
{
	specular[0] = r;
	specular[1] = g;
	specular[2] = b;
}

void PhongMaterial::setColor(vec3 color)
{
	setAmbient(color);
	setDiffuse(color);
	setSpecular(color);
}

void PhongMaterial::setColor(float r, float g, float b)
{
	setAmbient(r, g, b);
	setDiffuse(r, g, b);
	setSpecular(r, g, b);
}

void PhongMaterial::setShininess(float shininess)
{
	this->shininess = shininess;
}
