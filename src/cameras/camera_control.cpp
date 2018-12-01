#include "camera_control.h"

CameraControl::CameraControl(Camera * camera, InputManager * ih)
	: camera_(camera)
	, ih_(ih)
{

}

void CameraControl::update()
{
	float speedX = glm::abs(ih_->mouseMoved.x * speedFactor);
	float speedY = glm::abs(ih_->mouseMoved.y * speedFactor);

	float angleX = ih_->mouseMoved.y * -glm::pow(speedY, sensetivity);
	float angleY = ih_->mouseMoved.x * -glm::pow(speedX, sensetivity);

	if (angleY > 100.0f) {
		console::info("over angle y %f %i", angleY, ih_->mouseMoved.x);
		console::info("details %i %i %i", ih_->mouseMoved.x, ih_->mouseStart.x, ih_->mouse.x);
		angleY = 10.0f;
	}

	if (angleX > 100.0f) {
		console::info("over angle x %i", angleX);
		console::info("details %i %i %i", ih_->mouseMoved.y, ih_->mouseStart.y, ih_->mouse.y);
		angleX = 10.0f;
	}

	vec3 cameraPosition = camera_->getPosition();
	vec3 cameraRotation = camera_->getRotation(); // remove link rotation
	vec3 cameraUp = camera_->getUp();
	vec3 front = vec3(0.0f);

	cameraRotation.x+= angleX;
	cameraRotation.y+= angleY;

	float radRotationX = glm::radians(cameraRotation.x);
	float radRotationY = glm::radians(cameraRotation.y);

	front.x = -glm::cos(radRotationX) * glm::sin(radRotationY);
	front.y = glm::sin(radRotationX);
	front.z = glm::cos(radRotationX) * glm::cos(radRotationY);
	front = glm::normalize(front);

	vec3 left = glm::normalize(glm::cross(front, cameraUp));

	if (ih_->isPress(InputManager::KEY_W)) cameraPosition-= speed * front;
	if (ih_->isPress(InputManager::KEY_S)) cameraPosition+= speed * front;
	if (ih_->isPress(InputManager::KEY_A)) cameraPosition+= left * speed;
	if (ih_->isPress(InputManager::KEY_D)) cameraPosition-= left * speed;
	if (ih_->isPress(InputManager::KEY_C)) cameraPosition-= vec3(0.0f, 0.1f, 0.0f);
	if (ih_->isPress(InputManager::KEY_SPACE)) cameraPosition+= vec3(0.0f, 0.1f, 0.0f);

	camera_->setRotation(cameraRotation);
	camera_->setPosition(cameraPosition);
}

void CameraControl::calcSensetivity(uint width, uint height, double dpi)
{
	speedFactor = static_cast<float>(dpi) / 1000.0f;
}
