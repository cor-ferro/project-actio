#include "arrow_helper.h"

namespace RenderHelpers
{

Arrow::Arrow(vec3 origin, vec3 direction, float length)
: Model(),
  origin_(origin),
  direction_(direction),
  length_(length)
{
	line = new Mesh();
	cone = new Mesh();

	addMesh(line);
	addMesh(cone);

	line->setDrawMode(MESH_DRAW_MODE_LINE);
	line->setDrawItem(MESH_DRAW_ITEM_ARRAY);

	std::vector<float> lineVertices {
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	float coneHeight = 0.3f;
	float coneSize = 0.06603f;

	std::vector<float> coneVertices {
		0.0f, coneHeight, 0.0f,
		-coneSize, 0.0f, -coneSize,
		coneSize, 0.0f, -coneSize,

		0.0f, coneHeight, 0.0f,
		0.0f, 0.0f, coneSize,
		-coneSize, 0.0f, -coneSize,

		0.0f, coneHeight, 0.0f,
		coneSize, 0.0f, -coneSize,
		0.0f, 0.0f, coneSize
	};

	line->geometry.addVertex(lineVertices);
	line->material.setColor(0.0f, 1.0f, 0.0f);

	cone->geometry.addVertex(coneVertices);
	cone->material.setColor(0.0f, 0.0f, 1.0f);

	setLength(length_);
	setOrigin(origin_);
	setDirection(direction_);

	// already setup?
//	line->setup();
//	cone->setup();
}

void Arrow::setOrigin(vec3 origin)
{
	origin_ = origin;
	line->setPosition(origin_);
	cone->setPosition(origin_ + vec3(0.0, length_, 0.0));
}

void Arrow::setDirection(vec3 direction)
{
	direction_ = direction;
	float radians = glm::acos(direction_.y);
	quat newQuat = glm::angleAxis(radians, direction_);

	line->setQuaternion(newQuat);
	cone->setQuaternion(newQuat);
}

void Arrow::setLength(float length)
{
	length_ = length;
	line->setScale(vec3(length_));
	cone->setPosition(origin_ + vec3(0.0, length_, 0.0));
}

void Arrow::freeMesh()
{
	delete line;
	delete cone;

	line = nullptr;
	cone = nullptr;
}

Arrow::~Arrow()
{
	freeMesh();
}

} // RenderHelpers
