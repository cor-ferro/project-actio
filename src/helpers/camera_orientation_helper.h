/*
 * camera_orientation_helper.h
 *
 *  Created on: Oct 12, 2017
 *      Author: demitriy
 */

#ifndef CAMERA_ORIENTATION_HELPER_H_
#define CAMERA_ORIENTATION_HELPER_H_

#include <vector>
#include <glm/gtc/quaternion.hpp>
#include "../lib/types.h"
#include "../lib/console.h"
#include "../cameras/camera.h"
#include "../core/model.h"
#include "../core/mesh.h"
#include "../core/vertex.h"
#include "../core/material.h"
#include "../core/model.h"
#include "../core/mesh.h"
#include "../core/vertex.h"

namespace Helpers
{
	struct CameraOrientation : Model {
		CameraOrientation(vec3 origin, vec3 direction, float length);
		~CameraOrientation();

		void setLength(float length);
		void setPosition(vec3 origin);
		void setDirection(vec3 direction);
		void setCamera(Camera * camera);

	protected:
		Mesh * lineUp;
		Mesh * lineLeft;
		Mesh * lineForward;

	private:
		vec3 origin_;
		vec3 direction_;
		float length_;
		Camera * camera_;
	};
} // Helpers

#endif /* CAMERA_ORIENTATION_HELPER_H_ */
