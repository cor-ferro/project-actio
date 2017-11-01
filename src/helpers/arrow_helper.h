/*
 * arrow_helper.h
 *
 *  Created on: Aug 20, 2017
 *      Author: demitriy
 */

#ifndef ARROW_HELPER_H_
#define ARROW_HELPER_H_

#include <vector>
#include <glm/gtc/quaternion.hpp>
#include "../lib/types.h"
#include "../lib/console.h"
#include "../core/model.h"
#include "../core/mesh.h"
#include "../core/vertex.h"
#include "../materials/material.h"
#include "../core/model.h"
#include "../core/mesh.h"

namespace RenderHelpers
{

struct Arrow : Model {
	Arrow(vec3 origin, vec3 direction, float length);
	~Arrow();

	void setLength(float length);
	void setOrigin(vec3 origin);
	void setDirection(vec3 direction);

protected:
	Mesh * line;
	Mesh * cone;

	void freeMesh();

private:
	vec3 origin_;
	vec3 direction_;
	float length_;
};

} // RenderHelpers


#endif /* ARROW_HELPER_H_ */
