#ifndef OBJECT_3D_H_
#define OBJECT_3D_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <foundation/PxTransform.h>
#include <foundation/PxVec3.h>
#include <foundation/PxQuat.h>
#include "../lib/console.h"
#include "../lib/types.h"
#include <math.h>

class Object3D {
public:
    Object3D();

    Object3D(const Object3D &other);

    ~Object3D();

    const vec3 &getPosition();

    const vec3 &getScale();

    void rotate(const quat &rotateQuat);

    void setAffineTransform(const vec3 &pos, const quat &rot, const vec3 &scale);

    void setPosition(const vec3 &vec);

    void setScale(const vec3 &vec);

    void setScale(const float &s);

    void setScale(const float &x, const float &y, const float &z);

    void scaleX(const float &s);

    void scaleY(const float &s);

    void scaleZ(const float &s);

    void setPosition(float x, float y, float z);

    void setPosition(const physx::PxVec3 &pos);

    void setQuaternion(const quat &newQuat);

    void setQuaternion(float x, float y, float z, float w);

    void setQuaternion(const physx::PxQuat &quat);

    void setRotation(const vec3 &vector);

    void setRotation(float x, float y, float z);

    void setTransform(const physx::PxTransform &transform);

    void rotate(vec3 vector, float angle);

    void rotate(float x, float y, float z, float angle);

    void rotateX(float angle);

    void rotateY(float angle);

    void rotateZ(float angle);

    const mat4 &getModelMatrix();

    void updateModelMatrix(bool force);

    vec3 position;
    vec3 scale;
    vec3 rotation;
    quat quaternion;

protected:
    mat4 modelMatrix;

private:
    bool needUpdateMatrix;
};

#endif /* OBJECT_3D_H_ */
