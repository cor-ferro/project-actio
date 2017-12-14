#include "camera_control.h"

CameraControl::CameraControl(Camera * camera) : camera_(camera)
{

}

void CameraControl::update()
{
	InputHandler &ih = InputHandler::instance();

	float cameraSpeed = 0.25f;
	float speedFactor = 10.0f;
	float sensetivity = 1.2f;

	float speedX = glm::abs(ih.mouseMoved.x / speedFactor);
	float speedY = glm::abs(ih.mouseMoved.y / speedFactor);

	float angleX = ih.mouseMoved.y * -glm::pow(speedY, sensetivity);
	float angleY = ih.mouseMoved.x * -glm::pow(speedX, sensetivity);

	if (angleY > 100.0f) {
		console::info("over angle y", angleY, ih.mouseMoved.x);
		console::info("details", ih.mouseMoved.x, ih.mouseStart.x, ih.mouse.x);
	}

	if (angleX > 100.0f) {
		console::info("over angle x", angleX);
		console::info("details", ih.mouseMoved.y, ih.mouseStart.y, ih.mouse.y);
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

	if (ih.isPress(KEY_W)) cameraPosition-= cameraSpeed * front;
	if (ih.isPress(KEY_S)) cameraPosition+= cameraSpeed * front;
	if (ih.isPress(KEY_A)) cameraPosition+= left * cameraSpeed;
	if (ih.isPress(KEY_D)) cameraPosition-= left * cameraSpeed;
	if (ih.isPress(KEY_C)) cameraPosition-= vec3(0.0f, 0.1f, 0.0f);
	if (ih.isPress(32)) cameraPosition+= vec3(0.0f, 0.1f, 0.0f);

	camera_->setRotation(cameraRotation);
	camera_->setPosition(cameraPosition);
}

