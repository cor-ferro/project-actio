#ifndef CAMERA_CONTROL_H_
#define CAMERA_CONTROL_H_

#include <iostream>
#include "glm/glm.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "camera.h"
#include "../lib/input_handler.h"
#include "../lib/types.h"
#include "../lib/input_handler.h"

struct CameraControl {
	CameraControl(Camera * camera, InputHandler * ih);

	void update();
private:
	Camera * camera_;
	InputHandler * ih_;
};

#endif
