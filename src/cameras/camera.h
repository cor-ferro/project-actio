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

namespace CameraParam {
	enum Param {
		FOV,
		ASPECT,
		NEAR,
		FAR,
		SPEED,
		FRICTION,
		LEFT,
		RIGHT,
		TOP,
		BOTTOM
	};
}

struct Camera {
	Camera();
	virtual ~Camera();

	virtual void updateProjection() = 0;
	virtual void setParam(CameraParam::Param, float value) = 0;
	virtual float getParam(CameraParam::Param) = 0;
	void lookAt(vec3 target);

	void setNear(float near);
	void setFar(float far);
	void setPosition(vec3 position);
	void setPosition(float x, float y, float z);
	void setRotation(vec3 rotation);
	void setRotation(float x, float y, float z);

	mat4& getProjection();
	vec3& getPosition();
	vec3& getRotation();
	const mat4& getRotationMat();
	vec3& getFront();
	vec3& getUp();
	vec3& getTarget();
	mat4& getView();

	vec3 position_;
	vec3 rotation_;
	mat4 rotationMat_;
	mat4 view_;
	vec3 target_;

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
	void setParam(CameraParam::Param, float value);
	float getParam(CameraParam::Param);
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
	void setParam(CameraParam::Param, float value);
	float getParam(CameraParam::Param);
private:
	float left_;
	float right_;
	float top_;
	float bottom_;
	float near_;
	float far_;
};

#endif
