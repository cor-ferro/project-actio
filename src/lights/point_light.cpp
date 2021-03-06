#include "point_light.h"

namespace Light {
	Point::Point()
		: ambient(vec3(1.0))
		, diffuse(vec3(1.0))
		, specular(vec3(1.0))
		, constant(1.0f)
		, linear(0.001f)
		, quadratic(0.012f)
	{}

	Point::Point(vec3 position, vec3 ambient, vec3 diffuse, vec3 specular)
		: position(position)
		, ambient(ambient)
		, diffuse(diffuse)
		, specular(specular)
		, constant(1.0f)
		, linear(0.001f)
		, quadratic(0.012f)
	{}

	void Point::setAmbient(vec3 ambient) { this->ambient = ambient; }
	void Point::setAmbient(float r, float g, float b)
	{
		ambient[0] = r;
		ambient[1] = g;
		ambient[2] = b;
	}

	void Point::setDiffuse(vec3 diffuse) { this->diffuse = diffuse; }
	void Point::setDiffuse(float r, float g, float b)
	{
		diffuse[0] = r;
		diffuse[1] = g;
		diffuse[2] = b;
	}

	void Point::setSpecular(vec3 specular) { this->specular = specular; }
	void Point::setSpecular(float r, float g, float b)
	{
		specular[0] = r;
		specular[1] = g;
		specular[2] = b;
	}

	vec3 Point::getPosition() { return position; }
	vec3 Point::getAmbient() { return ambient; }
	vec3 Point::getDiffuse() { return diffuse; }
	vec3 Point::getSpecular() { return specular; }

	void Point::setPosition(vec3 position)
	{
		this->position = position;
	}

	void Point::setAttenuation(float constant, float linear, float quadratic)
	{
		this->constant = constant;
		this->linear = linear;
		this->quadratic = quadratic;
	}

	float Point::getRadius() {
		float lightMax = glm::max(diffuse.r, diffuse.g, diffuse.b);

		float colorDiff = (constant - (256.0f / 5.0f) * lightMax);
		float linearSquare = glm::sqrt(linear * linear - 4 * quadratic * colorDiff);
		float radius = (-linear + linearSquare) / (2 * quadratic);

		return radius;
	};
}
