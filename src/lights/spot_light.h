/*
 * spot_light.h
 *
 *  Created on: Aug 30, 2017
 *      Author: demitriy
 */

#ifndef SPOT_LIGHT_H_
#define SPOT_LIGHT_H_

#include "light.h"
#include "../lib/types.h"

namespace Light {
	struct Spot {
		Spot();
		Spot(vec3 ambient, vec3 diffuse, vec3 specular);

		void setAmbient(vec3 ambient);
		void setAmbient(float r, float g, float b);

		void setDiffuse(vec3 diffuse);
		void setDiffuse(float r, float g, float b);

		void setSpecular(vec3 specular);
		void setSpecular(float r, float g, float b);

		vec3 getAmbient();
		vec3 getDiffuse();
		vec3 getSpecular();

		void setPosition(vec3 position);
		void setDirection(vec3 direction);
		void setAttenuation(float constant, float linear, float quadratic);
		void setCutoff(float cutOff, float outerCutOff);

		vec3 ambient;
		vec3 diffuse;
		vec3 specular;
		vec3 position;
		vec3 direction;
		float constant;
		float linear;
		float quadratic;
		float cutOff;
		float outerCutOff;
	};
}


#endif /* SPOT_LIGHT_H_ */
