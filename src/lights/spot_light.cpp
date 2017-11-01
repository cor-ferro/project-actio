#include "spot_light.h"

namespace Renderer {
namespace Light {

Spot::Spot()
: ambient(vec3(1.0)),
  diffuse(vec3(1.0)),
  specular(vec3(1.0))
{

}

Spot::Spot(vec3 ambient, vec3 diffuse, vec3 specular)
: ambient(ambient),
  diffuse(diffuse),
  specular(specular)
{

}

void Spot::setAmbient(vec3 ambient) { this->ambient = ambient; }
void Spot::setAmbient(float r, float g, float b)
{
	ambient[0] = r;
	ambient[1] = g;
	ambient[2] = b;
}

void Spot::setDiffuse(vec3 diffuse) { this->diffuse = diffuse; }
void Spot::setDiffuse(float r, float g, float b)
{
	diffuse[0] = r;
	diffuse[1] = g;
	diffuse[2] = b;
}

void Spot::setSpecular(vec3 specular) { this->specular = specular; }
void Spot::setSpecular(float r, float g, float b)
{
	specular[0] = r;
	specular[1] = g;
	specular[2] = b;
}

vec3 Spot::getAmbient() { return ambient; }
vec3 Spot::getDiffuse() { return diffuse; }
vec3 Spot::getSpecular() { return specular; }

void Spot::setPosition(vec3 position)
{
	this->position = position;
}

void Spot::setDirection(vec3 direction)
{
	this->direction = direction;
}

void Spot::setAttenuation(float constant, float linear, float quadratic)
{
	this->constant = constant;
	this->linear = linear;
	this->quadratic = quadratic;
}

void Spot::setCutoff(float cutOff, float outerCutOff)
{
	this->cutOff = cutOff;
	this->outerCutOff = outerCutOff;
}

} // Light
} // Renderer
