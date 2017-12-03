#include "camera_orientation_helper.h"

namespace Helpers
{

	CameraOrientation::CameraOrientation(vec3 origin, vec3 direction, float length)
		: Model(),
	origin_(origin),
	direction_(direction),
	length_(length)
	{
		lineUp = new Mesh();
		lineLeft = new Mesh();
		lineForward = new Mesh();

		addMesh(lineUp);
		addMesh(lineLeft);
		addMesh(lineForward);

		lineUp->setDrawType(Mesh_Draw_Line);
		lineLeft->setDrawType(Mesh_Draw_Line);
		lineForward->setDrawType(Mesh_Draw_Line);

		std::vector<float> lineVerticesUp { 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f };
		std::vector<float> lineVerticesLeft { 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f };
		std::vector<float> lineVerticesForward { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f };

		lineUp->geometry.addVertex(lineVerticesUp);
		lineUp->material.setColor(0.0f, 1.0f, 0.0f);

		lineLeft->geometry.addVertex(lineVerticesLeft);
		lineLeft->material.setColor(0.0f, 1.0f, 0.0f);

		lineForward->geometry.addVertex(lineVerticesForward);
		lineForward->material.setColor(0.0f, 1.0f, 0.0f);

		setLength(length_);
		setPosition(origin_);
		setDirection(direction_);

		// already setup?
		//	line->setup();
		//	cone->setup();
	}

	void CameraOrientation::setPosition(vec3 origin)
	{
		origin_ = origin;
		lineUp->setPosition(origin_);
		lineLeft->setPosition(origin_);
		lineForward->setPosition(origin_);
	}

	void CameraOrientation::setDirection(vec3 direction)
	{
		vec3 dir = glm::radians(direction * -1.0f);

		lineUp->setRotation(dir);
		lineLeft->setRotation(dir);
		lineForward->setRotation(dir);
	}

	void CameraOrientation::setLength(float length)
	{
		length_ = length;
		lineUp->setScale(vec3(length_));
		lineLeft->setScale(vec3(length_));
		lineForward->setScale(vec3(length_));
	}

	void CameraOrientation::freeMesh()
	{
		delete lineUp;
		delete lineLeft;
		delete lineForward;

		lineUp = nullptr;
		lineLeft = nullptr;
		lineForward = nullptr;
	}

	CameraOrientation::~CameraOrientation()
	{
		freeMesh();
	}

} // Helpers
