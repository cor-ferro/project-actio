#include "object3D.h"

Object3D::Object3D()
        : position(vec3(0.0f))
        , rotation(vec3(0.0f, 0.0f, 0.0f))
        , scale(vec3(1.0f))
        , quaternion(quat(1.0f, 0.0f, 0.0f, 0.0f))
        , modelMatrix(mat4(1.0f))
        , needUpdateMatrix(true) {}

Object3D::Object3D(const Object3D &other) = default;

Object3D::~Object3D() = default;

void Object3D::rotate(const quat &rotateQuat) {
    modelMatrix = modelMatrix * glm::mat4_cast(rotateQuat);
}

void Object3D::setAffineTransform(const vec3 &pos, const quat &rot, const vec3 &scale) {
    setPosition(pos);
    setQuaternion(rot);
    setScale(scale);
}

void Object3D::setPosition(const vec3 &vector) {
    position = vector;

    needUpdateMatrix = true;
}

void Object3D::setPosition(float x, float y, float z) {
    position.x = x;
    position.y = y;
    position.z = z;

    needUpdateMatrix = true;
}

void Object3D::setPosition(const physx::PxVec3 &pos) {
    setPosition(pos.x, pos.y, pos.z);
}

void Object3D::setScale(const vec3 &vector) {
    scale = vector;

    needUpdateMatrix = true;
}

void Object3D::setScale(const float &s) {
    setScale(vec3(s));
}

void Object3D::setScale(const float &x, const float &y, const float &z) {
    scale.x = x;
    scale.y = y;
    scale.z = z;

    needUpdateMatrix = true;
}

void Object3D::setRotation(float x, float y, float z) {
    quat newQuat(vec3(x, y, z));
    setQuaternion(newQuat);
    needUpdateMatrix = true;
}

void Object3D::setRotation(const vec3 &vector) {
    quat newQuat(vector);
    setQuaternion(newQuat);
    needUpdateMatrix = true;
}

void Object3D::setQuaternion(const quat &newQuat) {
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

void Object3D::setQuaternion(const physx::PxQuat &quat) {
    setQuaternion(quat.x, quat.y, quat.z, quat.w);
}

void Object3D::setTransform(const physx::PxTransform &transform) {
    setPosition(transform.p);
    setQuaternion(transform.q);
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

const vec3 &Object3D::getPosition() {
    return position;
}

const vec3 &Object3D::getScale() {
    return scale;
}

void Object3D::updateModelMatrix(bool force = false) {
    if (needUpdateMatrix || force) {
        // console::info("update matrix");
        modelMatrix = mat4(1.0f);

        mat4 translateMatrix = glm::translate(modelMatrix, position);
        mat4 rotateMatrix = glm::mat4_cast(quaternion);
        mat4 scaleMatrix = glm::scale(modelMatrix, scale);

        modelMatrix = translateMatrix * rotateMatrix * scaleMatrix;
        needUpdateMatrix = false;
    }
}

void Object3D::scaleX(const float &s) {
    scale.x = s;
    needUpdateMatrix = true;
}

void Object3D::scaleY(const float &s) {
    scale.y = s;
    needUpdateMatrix = true;
}

void Object3D::scaleZ(const float &s) {
    scale.z = s;
    needUpdateMatrix = true;
}
