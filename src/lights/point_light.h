/*
 * point_light.h
 *
 *  Created on: Aug 29, 2017
 *      Author: demitriy
 */

#ifndef POINT_LIGHT_H_
#define POINT_LIGHT_H_


#include "light.h"
#include "../lib/types.h"
#include <glm/gtx/extended_min_max.hpp>

namespace Light {
	struct Point {
		Point();
		Point(vec3 position, vec3 ambient, vec3 diffuse, vec3 specular);

		void setAmbient(vec3 ambient);
		void setAmbient(float r, float g, float b);

		void setDiffuse(vec3 diffuse);
		void setDiffuse(float r, float g, float b);

		void setSpecular(vec3 specular);
		void setSpecular(float r, float g, float b);

		vec3 getAmbient();
		vec3 getDiffuse();
		vec3 getSpecular();
		vec3 getPosition();

		void setPosition(vec3 position);
		void setAttenuation(float constant, float linear, float quadratic);

		float getRadius();

		vec3 ambient;
		vec3 diffuse;
		vec3 specular;
		vec3 position;
		float constant;
		float linear;
		float quadratic;
	};
} // Light


#endif /* POINT_LIGHT_H_ */
