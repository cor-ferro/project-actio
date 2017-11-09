#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <string>
#include <vector>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include "../lib/console.h"
#include "../lib/types.h"
#include "../core/texture.h"
#include "../resources/resources.h"

struct Material {
	Material();
	Material(Texture);
	~Material();

	void setWireframe(bool);
	void setupTextures();

	void addTexture(Texture texture);
	const std::vector<Texture>& getTextures();
	bool wireframe;

protected:
	std::vector<Texture> textures_;
};

struct PhongMaterial : Material {
	PhongMaterial();
	PhongMaterial(Texture texture);
	PhongMaterial(aiMaterial * material, const Resource::Assimp * assimpResource);
	~PhongMaterial();

	void initFromAi(aiMaterial * material, const Resource::Assimp * assimpResource);
	void initEmptyTextures();

	void setAmbient(vec3 ambient);
	void setAmbient(float r, float g, float b);

	void setDiffuse(vec3 diffuse);
	void setDiffuse(float r, float g, float b);

	void setSpecular(vec3 specular);
	void setSpecular(float r, float g, float b);

	void setColor(vec3 color);
	void setColor(float r, float g, float b);

	void setShininess(float shininess);

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct ShaderMaterial : Material {

};


#endif
