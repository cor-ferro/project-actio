#ifndef CAMERA_CONTROL_H_
#define CAMERA_CONTROL_H_

#include <iostream>
#include "glm/glm.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "camera.h"
#include "../lib/input_manager.h"
#include "../lib/types.h"
#include "../lib/input_manager.h"

struct CameraControl {
	CameraControl(Camera * camera, InputManager * ih);

	void update();
	void calcSensetivity(uint width, uint height, double dpi);
private:
	Camera * camera_;
	InputManager * ih_;

	float sensetivity = 1.0f;
	float speedFactor = 1.0f;
	float speed = 1.0f;
};

#endif
