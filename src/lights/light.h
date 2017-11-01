#ifndef LIGHT_H_
#define LIGHT_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../core/object3D.h"
#include "../lib/types.h"

namespace Renderer {
namespace Light {

struct Light {
	Light();
	Light(vec3 ambient, vec3 diffuse, vec3 specular);

	void setAmbient(vec3 ambient);
	void setAmbient(float r, float g, float b);

	void setDiffuse(vec3 diffuse);
	void setDiffuse(float r, float g, float b);

	void setSpecular(vec3 specular);
	void setSpecular(float r, float g, float b);

	vec3 getAmbient();
	vec3 getDiffuse();
	vec3 getSpecular();

	// keep same order
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

}
}

#endif
