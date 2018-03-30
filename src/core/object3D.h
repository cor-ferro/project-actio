#ifndef OBJECT_3D_H_
#define OBJECT_3D_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include "../lib/console.h"
#include "../lib/types.h"
#include <math.h>

struct Object3D {
	Object3D();
	Object3D(const Object3D& other);
	~Object3D();

	vec3 getPosition();

	void rotate(quat rotateQuat);
	void setPosition(vec3 vec);
	void setScale(vec3 vec);
	void setScale(float s);
	void setPosition(float x, float y, float z);
	void setQuaternion(quat& newQuat);
	void setQuaternion(float x, float y, float z, float w);
	void setRotation(vec3& vector);
	void setRotation(float x, float y, float z);

	void rotate(vec3 vector, float angle);
	void rotate(float x, float y, float z, float angle);
	void rotateX(float angle);
	void rotateY(float angle);
	void rotateZ(float angle);

	const mat4& getModelMatrix();
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
