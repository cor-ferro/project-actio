#include "bounding_box_helper.h"

namespace RenderHelpers
{

BoundingBox::BoundingBox(Model * model)
	: model_(model)
{

}

void BoundingBox::determinateBounding()
{
	const ModelMeshes& modelMeshes = model_->getMeshes();

	for (auto mesh = modelMeshes.begin(); mesh != modelMeshes.end(); mesh++)
	{
		std::vector<Vertex>* vertices = (*mesh)->geometry.getVertices();

		float minX, minY, minZ;
		float maxX, maxY, maxZ;

		minX = maxX = (*vertices).front().Position.x;
		minY = maxY = (*vertices).front().Position.y;
		minZ = maxZ = (*vertices).front().Position.z;

		for (const auto& vertex : (*vertices))
		{
			if (vertex.Position.x < minX) minX = vertex.Position.x;
			if (vertex.Position.x > maxX) maxX = vertex.Position.x;

			if (vertex.Position.y < minY) minY = vertex.Position.y;
			if (vertex.Position.y > maxY) maxY = vertex.Position.y;

			if (vertex.Position.z < minZ) minZ = vertex.Position.z;
			if (vertex.Position.z > maxZ) maxZ = vertex.Position.z;
		}
	}
}

}
