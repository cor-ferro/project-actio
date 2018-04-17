/*
 * ag.h
 *
 *  Created on: Oct 14, 2017
 *      Author: demitriy
 */

#ifndef AG_H_
#define AG_H_

#include "./core/model.h"
#include "./core/mesh.h"
#include "./core/geometry.h"
#include "./cameras/camera.h"
#include "./materials/material.h"
#include "./lights/direction_light.h"
#include "./lights/point_light.h"
#include "./lights/spot_light.h"
#include "./helpers/arrow_helper.h"
#include "./helpers/camera_orientation_helper.h"
#include "./resources/resources.h"
#include "./lib/image_loader.h"

namespace AG
{
	typedef Helpers::Arrow HelperArrow;
	typedef Helpers::CameraOrientation HelperCameraOrientation;

	PerspectiveCamera * createPerspectiveCamera(float, float, float, float);
	OrthographicCamera * createOrthographicCamera(float, float, float, float, float, float);

	namespace Models {
		// Model * create(Resource::File);
		Model * create(Model::Config& modelConfig);
	}

	namespace Helper {
		HelperArrow * arrow(vec3, vec3, float);
		HelperCameraOrientation * cameraOrientation(vec3, vec3, float);
	}

	namespace Light {
		::Light::Directional * directional();
		::Light::Point * point();
		::Light::Spot * spot();

		::Light::Directional * directional(vec3, vec3, vec3);
		::Light::Point * point(vec3, vec3, vec3, vec3);
		::Light::Spot * spot(vec3, vec3, vec3);
	}
}


#endif /* AG_H_ */
