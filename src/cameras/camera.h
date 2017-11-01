#ifndef CAMERA_H_
#define CAMERA_H_

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "../lib/console.h"
#include "../lib/types.h"
#include <iostream>

struct Camera {
	Camera();
	virtual ~Camera();

	virtual void updateProjection() = 0;
	void lookAt(vec3 target);

	void setFov(float fov);
	void setAspect(float aspect);
	void setPosition(vec3 position);
	void setPosition(float x, float y, float z);
	void setRotation(vec3 rotation);
	void setRotation(float x, float y, float z);

	mat4& getProjection();
	vec3& getPosition();
	vec3& getRotation();
	vec3& getFront();
	vec3& getUp();
	mat4& getView();

	vec3 position_;
	vec3 rotation_;
	mat4 view_;

protected:
	void updateViewMatrix();

	mat4 projection_;

	vec3 front_;
	vec3 up_;

	float speed_;
	float friction_;
};

struct PerspectiveCamera : Camera {
	PerspectiveCamera(float fov, float aspect, float near, float far);
	~PerspectiveCamera();
	void updateProjection();
private:
	float fov_;
	float aspect_;
	float near_;
	float far_;
};

struct OrthographicCamera : Camera {
	OrthographicCamera(float left, float right, float top, float bottom, float near, float far);
	~OrthographicCamera();
	void updateProjection();
private:
	float left_;
	float right_;
	float top_;
	float bottom_;
	float near_;
	float far_;
};

#endif
