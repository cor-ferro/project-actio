#include "object3D.h"

Object3D::Object3D()
        : position(vec3(0.0f))
        , rotation(vec3(0.0f, 0.0f, 0.0f))
        , scale(vec3(1.0f))
        , quaternion(quat(1.0f, 0.0f, 0.0f, 0.0f))
        , modelMatrix(mat4(1.0f))
        , needUpdateMatrix(true) {}

Object3D::Object3D(const Object3D &other)
        : position(other.position)
        , scale(other.scale)
        , rotation(other.rotation)
        , quaternion(other.quaternion)
        , modelMatrix(other.modelMatrix)
        , needUpdateMatrix(other.needUpdateMatrix) {}

Object3D::~Object3D() {}

void Object3D::rotate(quat rotateQuat) {
    modelMatrix = modelMatrix * glm::mat4_cast(rotateQuat);
}

void Object3D::setPosition(vec3 vector) {
    position = vector;

    needUpdateMatrix = true;
}

void Object3D::setPosition(float x, float y, float z) {
    position.x = x;
    position.y = y;
    position.z = z;

    needUpdateMatrix = true;
}

void Object3D::setScale(vec3 vector) {
    scale = vector;

    needUpdateMatrix = true;
}

void Object3D::setScale(float s) {
    setScale(vec3(s));
}

void Object3D::setRotation(float x, float y, float z) {
    quat newQuat(vec3(x, y, z));
    setQuaternion(newQuat);
    needUpdateMatrix = true;
}

void Object3D::setRotation(vec3 &vector) {
    quat newQuat(vector);
    setQuaternion(newQuat);
    needUpdateMatrix = true;
}

void Object3D::setQuaternion(quat &newQuat) {
    quaternion = newQuat;
    needUpdateMatrix = true;
}

void Object3D::setQuaternion(float x, float y, float z, float w) {
    quaternion.w = w;
    quaternion.x = x;
    quaternion.y = y;
    quaternion.z = z;
    needUpdateMatrix = true;
}

void Object3D::rotate(float x, float y, float z, float angle) {
    quaternion = glm::rotate(quaternion, angle, vec3(x, y, z));
    needUpdateMatrix = true;
}

void Object3D::rotate(vec3 vector, float angle) {
    quaternion = glm::rotate(quaternion, angle, vector);
    needUpdateMatrix = true;
}

void Object3D::rotateX(float angle) {
    quaternion = glm::rotate(quaternion, angle, vec3(1.0, 0.0, 0.0));
    needUpdateMatrix = true;
}

void Object3D::rotateY(float angle) {
    quaternion = glm::rotate(quaternion, angle, vec3(0.0, 1.0, 0.0));
    needUpdateMatrix = true;
}

void Object3D::rotateZ(float angle) {
    quaternion = glm::rotate(quaternion, angle, vec3(0.0, 0.0, 1.0));
    needUpdateMatrix = true;
}

const mat4 &Object3D::getModelMatrix() {
    updateModelMatrix(false);

    return modelMatrix;
}

vec3 Object3D::getPosition() {
    return position;
}

void Object3D::updateModelMatrix(bool force = false) {
    if (true) { // todo@ add logic with needUpdateMatrix and force
        // console::info("update matrix");
        modelMatrix = mat4(1.0f);

        mat4 translateMatrix = glm::translate(modelMatrix, position);
        mat4 rotateMatrix = glm::mat4_cast(quaternion);
        mat4 scaleMatrix = glm::scale(modelMatrix, scale);

        modelMatrix = translateMatrix * rotateMatrix * scaleMatrix;
        needUpdateMatrix = false;
    }
}
