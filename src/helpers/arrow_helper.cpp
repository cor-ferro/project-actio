#include "arrow_helper.h"

namespace Helpers
{

	Arrow::Arrow(vec3 origin, vec3 direction, float length)
		: Model()
		, origin_(origin)
		, direction_(direction)
		, length_(length)
	{
		line = Mesh::Create();
		cone = Mesh::Create();

		addMesh(line);
		addMesh(cone);

		line->setDrawType(Mesh_Draw_Line);

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
		line->material->setColor(0.0f, 1.0f, 0.0f);

		cone->geometry.addVertex(coneVertices);
		cone->material->setColor(0.0f, 0.0f, 1.0f);

		setLength(length_);
		setOrigin(origin_);
		setDirection(direction_);
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

	Arrow::~Arrow() {}

} // Helpers
