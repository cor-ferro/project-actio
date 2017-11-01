#include "object3D.h"

Object3D::Object3D()
{
//	console::info("create 3d object");
	countObjects++;

	position = vec3(0.0f);
	rotation = vec3(1.0f, 0.0f, 0.0f);
	scale = vec3(1.0f);

	modelMatrix = mat4();
	needUpdateMatrix = true;
}

Object3D::~Object3D()
{
	countObjects--;
}

void Object3D::rotate(quat rotateQuat)
{
	modelMatrix = modelMatrix * glm::toMat4(rotateQuat);
}

void Object3D::setPosition(vec3 vector)
{
	position = vector;

	needUpdateMatrix = true;
}

void Object3D::setPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;

	needUpdateMatrix = true;
}

void Object3D::setScale(vec3 vector)
{
	scale = vector;

	needUpdateMatrix = true;
}

void Object3D::setRotation(float x, float y, float z)
{
	quat newQuat(vec3(x, y, z));
	setQuaternion(newQuat);
	needUpdateMatrix = true;
}

void Object3D::setRotation(vec3& vector)
{
	quat newQuat(vector);
	setQuaternion(newQuat);
	needUpdateMatrix = true;
}

void Object3D::setQuaternion(quat& newQuat)
{
	quaternion = newQuat;
	needUpdateMatrix = true;
}

void Object3D::rotate(float x, float y, float z, float angle)
{
	quaternion = glm::rotate(quaternion, angle, vec3(x, y, z));
	needUpdateMatrix = true;
}

void Object3D::rotate(vec3 vector, float angle)
{
	quaternion = glm::rotate(quaternion, angle, vector);
	needUpdateMatrix = true;
}

void Object3D::rotateX(float angle)
{
	quaternion = glm::rotate(quaternion, angle, vec3(1.0, 0.0, 0.0));
	needUpdateMatrix = true;
}

void Object3D::rotateY(float angle)
{
	quaternion = glm::rotate(quaternion, angle, vec3(0.0, 1.0, 0.0));
	needUpdateMatrix = true;
}

void Object3D::rotateZ(float angle)
{
	quaternion = glm::rotate(quaternion, angle, vec3(0.0, 0.0, 1.0));
	needUpdateMatrix = true;
}

mat4 Object3D::getModelMatrix()
{
	updateModelMatrix(false);

	return modelMatrix;
}

vec3 Object3D::getPosition()
{
	return position;
}

void Object3D::updateModelMatrix(bool force = false)
{
	if (force == true || needUpdateMatrix == true)
	{
//		console::info("update matrix");
		modelMatrix = mat4(1.0);

		mat4 translateMatrix = glm::translate(modelMatrix, position);
		mat4 rotateMatrix = glm::toMat4(quaternion);
		mat4 scaleMatrix = glm::scale(modelMatrix, scale);

		modelMatrix = translateMatrix * rotateMatrix * scaleMatrix;
		needUpdateMatrix = false;
	}
}
