#include "material.h"

Material::Material()
{
	wireframe = false;
}

Material::Material(Texture texture)
	: wireframe(false)
{
	addTexture(texture);
}

Material::~Material()
{

}

void Material::addTexture(Texture texture)
{
	textures_.push_back(texture);
}

void Material::setupTextures()
{
	for (Texture& texture : textures_)
	{
		switch(texture.type)
		{
			case Texture_Diffuse:
			case Texture_Specular:
			case Texture_Height:
			case Texture_Normal:
				texture.setup2d();
				break;
			case Texture_Cube:
				texture.setupCube();
				break;
			default:
				console::warn("setup unknown texture type", texture.type, texture.name);
		}
	}
}

void Material::setWireframe(bool flag)
{
	wireframe = flag;
}

const MaterialTextures& Material::getTextures()
{
	return textures_;
}

PhongMaterial::PhongMaterial(Texture texture)
	: shininess(1.0f)
	, noLight(false)
	, Material(texture)
{}

PhongMaterial::PhongMaterial()
	: shininess(1.0f)
	, noLight(false)
{}

PhongMaterial::PhongMaterial(aiMaterial * material, const Resource::Assimp * assimpResource)
	: shininess(1.0f)
	, noLight(false)
{
	initFromAi(material, assimpResource);
}

PhongMaterial::~PhongMaterial() {};

void PhongMaterial::initFromAi(aiMaterial * material, const Resource::Assimp * assimpResource)
{
	if (!material) {
		return;
	}

	aiColor3D diffuseColor(0.0f, 0.0f, 0.0f);
	aiColor3D specularColor(0.0f, 0.0f, 0.0f);
	aiColor3D ambientColor(0.0f, 0.0f, 0.0f);
	float shininessValue = 0.0f;
	float reflectivity = 0.0f;
	float refracti = 0.0f;
	float colorReflective = 0.0f;
	bool isWireframe = false;

	// @todo: uncomment get material color and fix shader
	//material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
	//material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
	//material->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor);
	material->Get(AI_MATKEY_SHININESS, shininessValue);
	material->Get(AI_MATKEY_ENABLE_WIREFRAME, wireframe);

	PhongMaterial meshMaterial;

	setDiffuse(diffuseColor.r, diffuseColor.g, diffuseColor.b);
	setSpecular(specularColor.r, specularColor.g, specularColor.b);
	setAmbient(ambientColor.r, ambientColor.g, ambientColor.b);
	setWireframe(isWireframe);
	setShininess(shininessValue);

	Texture diffuseMap  = Texture::loadFromMaterial(material, aiTextureType_DIFFUSE, assimpResource);
	Texture specularMap = Texture::loadFromMaterial(material, aiTextureType_SPECULAR, assimpResource);
	Texture heightMap   = Texture::loadFromMaterial(material, aiTextureType_HEIGHT, assimpResource);
	Texture normalMap   = Texture::loadFromMaterial(material, aiTextureType_NORMALS, assimpResource);

	textures_.clear();
	textures_.reserve(4);
	textures_.push_back(diffuseMap);
	textures_.push_back(specularMap);
	textures_.push_back(heightMap);
	textures_.push_back(normalMap);
}

void PhongMaterial::initEmptyTextures()
{
	Texture diffuseMap  = Texture::White();
	Texture specularMap = Texture::White();
	Texture heightMap   = Texture::White();
	Texture normalMap   = Texture::White();

	diffuseMap.setAsDiffuse();
	specularMap.setAsSpecular();
	heightMap.setAsHeight();
	normalMap.setAsNormal();

	textures_.clear();
	textures_.reserve(4);
	textures_.push_back(diffuseMap);
	textures_.push_back(specularMap);
	textures_.push_back(heightMap);
	textures_.push_back(normalMap);
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

void PhongMaterial::setNoLight(bool isNoLight)
{
	noLight = isNoLight;
}

bool PhongMaterial::isNoLight()
{
	return noLight;
}
