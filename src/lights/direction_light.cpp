#include "direction_light.h"

namespace Renderer {
namespace Light {

Directional::Directional()
: ambient(vec3(1.0)),
  diffuse(vec3(1.0)),
  specular(vec3(1.0))
{

}

Directional::Directional(vec3 ambient, vec3 diffuse, vec3 specular)
: ambient(ambient),
  diffuse(diffuse),
  specular(specular)
{

}

void Directional::setAmbient(vec3 ambient) { this->ambient = ambient; }
void Directional::setAmbient(float r, float g, float b)
{
	ambient[0] = r;
	ambient[1] = g;
	ambient[2] = b;
}

void Directional::setDiffuse(vec3 diffuse) { this->diffuse = diffuse; }
void Directional::setDiffuse(float r, float g, float b)
{
	diffuse[0] = r;
	diffuse[1] = g;
	diffuse[2] = b;
}

void Directional::setSpecular(vec3 specular) { this->specular = specular; }
void Directional::setSpecular(float r, float g, float b)
{
	specular[0] = r;
	specular[1] = g;
	specular[2] = b;
}

void Directional::setDirection(vec3 direction) { this->direction = direction; }
void Directional::setDirection(float r, float g, float b)
{
	direction[0] = r;
	direction[1] = g;
	direction[2] = b;
}

vec3 Directional::getAmbient() { return ambient; }
vec3 Directional::getDiffuse() { return diffuse; }
vec3 Directional::getSpecular() { return specular; }
vec3 Directional::getDirection() { return direction; }

} // Light
} // Renderer
