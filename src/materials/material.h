#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <string>
#include <vector>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include "../lib/console.h"
#include "../lib/types.h"
#include "../lib/colors.h"
#include "../core/texture.h"
#include "../resources/resources.h"

typedef std::vector<Texture> MaterialTextures;

namespace Material {
	struct Base {
		Base();
		Base(Texture);
		~Base();

		void destroy();

		void setWireframe(bool);
		void setupTextures();

		void addTexture(Texture texture);
		const MaterialTextures& getTextures();
		bool wireframe = false;

	protected:
		MaterialTextures textures_;
	};

	struct Phong : Base {
		Phong();
		Phong(Texture texture);
		~Phong();

		void initFromAi(const aiMaterial * material, const Resource::Assimp * assimpResource, std::unordered_map<std::string, ImageLoader::Data>& images);
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

		void setNoLight(bool isNoLight);
		bool isNoLight();

		vec3 ambient;
		vec3 diffuse;
		vec3 specular;
		float shininess;

		bool noLight;
	};
}

#endif
