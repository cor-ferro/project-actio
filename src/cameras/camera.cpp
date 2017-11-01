#include "camera.h"

Camera::Camera()
{
	position_ = vec3(0.0f, 5.0f, 10.0f);
	rotation_ = vec3(0.0f, 0.0f, 0.0f);
	front_ = vec3(0.0f, 0.0f, -3.0f);
	up_ = vec3(0.0f, 1.0f,  0.0f);

	speed_ = 1.0f;
	friction_ = 0.0f;

	console::info("create camera");
}

Camera::~Camera() {}

void Camera::lookAt(vec3 target)
{
//	front_ = target;
//	view_ = glm::lookAt(position_, position_ + front_, up_); // todo: not work, if need absolute direction(without position_)
	updateViewMatrix();
}

void Camera::setPosition(vec3 position)
{
	position_ = position;
	updateViewMatrix();
}

void Camera::setPosition(float x, float y, float z)
{
	position_.x = x;
	position_.y = y;
	position_.z = z;
	updateViewMatrix();
}

void Camera::setRotation(vec3 rotation)
{
	rotation_ = rotation;
	updateViewMatrix();
}

void Camera::setRotation(float x, float y, float z)
{
	rotation_.x = x;
	rotation_.y = y;
	rotation_.z = z;
	updateViewMatrix();
}

void Camera::updateViewMatrix()
{
	mat4 rotM = mat4(1.0f);
	mat4 transM;

	rotM = glm::rotate(rotM, glm::radians(rotation_.x), vec3(1.0f, 0.0f, 0.0f));
	rotM = glm::rotate(rotM, glm::radians(rotation_.y), vec3(0.0f, 1.0f, 0.0f));
	rotM = glm::rotate(rotM, glm::radians(rotation_.z), vec3(0.0f, 0.0f, 1.0f));

	transM = glm::translate(mat4(1.0f), position_ * -1.0f);

//	view_ = glm::lookAt(position_, position_ + front_, up_);
	view_ = rotM * transM; // for 3d person reverse operands
}

mat4& Camera::getView() { return view_; }
vec3& Camera::getPosition() { return position_; }
vec3& Camera::getRotation() { return rotation_; }
vec3& Camera::getFront() { return front_; }
vec3& Camera::getUp() { return up_; }
mat4& Camera::getProjection() { return projection_; }


PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float near, float far)
: fov_(fov),
  aspect_(aspect),
  near_(near),
  far_(far)
{
	updateProjection();
}

PerspectiveCamera::~PerspectiveCamera() {}

void PerspectiveCamera::updateProjection()
{
	projection_ = glm::perspective(glm::radians(fov_), aspect_, near_, far_);
}



OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float near, float far)
: left_(left),
  right_(right),
  bottom_(bottom),
  top_(top),
  near_(near),
  far_(far)
{
	updateProjection();
}

OrthographicCamera::~OrthographicCamera() {}

void OrthographicCamera::updateProjection()
{
	projection_ = glm::ortho(left_, right_, bottom_, top_, near_, far_);
}


