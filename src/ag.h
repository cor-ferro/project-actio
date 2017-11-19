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
	typedef RenderHelpers::Arrow HelperArrow;
	typedef RenderHelpers::CameraOrientation HelperCameraOrientation;

	typedef Renderer::Light::Directional LightDirectional;
	typedef Renderer::Light::Point LightPoint;
	typedef Renderer::Light::Spot LightSpot;

	PerspectiveCamera * createPerspectiveCamera(float, float, float, float);
	OrthographicCamera * createOrthographicCamera(float, float, float, float, float, float);

	namespace Models {
		// Model * create(Resource::File);
		Model * create(ModelConfig& modelConfig);
		Model * skybox(std::vector<Resource::File> faces);
	}

	namespace Helper {
		HelperArrow * arrow(vec3, vec3, float);
		HelperCameraOrientation * cameraOrientation(vec3, vec3, float);
	}

	namespace Light {
		LightDirectional * directional(vec3, vec3, vec3);
		LightPoint * point(vec3, vec3, vec3);
		LightSpot * spot(vec3, vec3, vec3);
	}
}


#endif /* AG_H_ */
