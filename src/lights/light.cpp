#include "light.h"

namespace Renderer {
namespace Light {

Light::Light()
: ambient(vec3(1.0)),
  diffuse(vec3(1.0)),
  specular(vec3(1.0))
{

}

Light::Light(vec3 ambient, vec3 diffuse, vec3 specular)
: ambient(ambient),
  diffuse(diffuse),
  specular(specular)
{

}

void Light::setAmbient(vec3 ambient) { this->ambient = ambient; }
void Light::setAmbient(float r, float g, float b)
{
	ambient[0] = r;
	ambient[1] = g;
	ambient[2] = b;
}

void Light::setDiffuse(vec3 diffuse) { this->diffuse = diffuse; }
void Light::setDiffuse(float r, float g, float b)
{
	diffuse[0] = r;
	diffuse[1] = g;
	diffuse[2] = b;
}

void Light::setSpecular(vec3 specular) { this->specular = specular; }
void Light::setSpecular(float r, float g, float b)
{
	specular[0] = r;
	specular[1] = g;
	specular[2] = b;
}

vec3 Light::getAmbient() { return ambient; }
vec3 Light::getDiffuse() { return diffuse; }
vec3 Light::getSpecular() { return specular; }

}
}
