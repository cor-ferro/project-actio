#include "bounding_box_helper.h"

namespace Helpers
{

	BoundingBox::BoundingBox(Model * model)
		: model_(model)
	{
		const ModelMeshes& meshes = model->getMeshes();

		for (auto mesh : meshes)
		{
			mesh->geometry.computeBoundingBox();
			const Math::Box3& meshBbox = mesh->geometry.getBoundingBox();

			Mesh * boxMesh = createBoxMesh(meshBbox);

			addMesh(boxMesh);
		}
	}

	Mesh * BoundingBox::createBoxMesh(const Math::Box3& boundingBox)
	{
		Mesh * mesh = Mesh::Create();

		mesh->setDrawType(Mesh_Draw_Line);
		mesh->setPosition(boundingBox.center);

		mesh->material->setWireframe(true);
		mesh->material->setDiffuse(0.0f, 1.0f, 0.0f);

		vec3 btlCorner = vec3(-1.0f, 1.0f, -1.0f);
		vec3 btrCorner = vec3(1.0f, 1.0f, -1.0f);
		vec3 bblCorner = vec3(-1.0f, -1.0f, -1.0f);
		vec3 bbrCorner = vec3(1.0f, -1.0f, -1.0f);

		vec3 ftlCorner = vec3(-1.0f, 1.0f, 1.0f);
		vec3 ftrCorner = vec3(1.0f, 1.0f, 1.0f);
		vec3 fblCorner = vec3(-1.0f, -1.0f, 1.0f);
		vec3 fbrCorner = vec3(1.0f, -1.0f, 1.0f);

		fblCorner.y*= boundingBox.radius.y;
		fbrCorner.y*= boundingBox.radius.y;
		bblCorner.y*= boundingBox.radius.y;
		bbrCorner.y*= boundingBox.radius.y;
		ftlCorner.y*= boundingBox.radius.y;
		ftrCorner.y*= boundingBox.radius.y;
		btlCorner.y*= boundingBox.radius.y;
		btrCorner.y*= boundingBox.radius.y;

		fblCorner.x*= boundingBox.radius.x;
		fbrCorner.x*= boundingBox.radius.x;
		bblCorner.x*= boundingBox.radius.x;
		bbrCorner.x*= boundingBox.radius.x;
		ftlCorner.x*= boundingBox.radius.x;
		ftrCorner.x*= boundingBox.radius.x;
		btlCorner.x*= boundingBox.radius.x;
		btrCorner.x*= boundingBox.radius.x;

		fblCorner.z*= boundingBox.radius.z;
		fbrCorner.z*= boundingBox.radius.z;
		bblCorner.z*= boundingBox.radius.z;
		bbrCorner.z*= boundingBox.radius.z;
		ftlCorner.z*= boundingBox.radius.z;
		ftrCorner.z*= boundingBox.radius.z;
		btlCorner.z*= boundingBox.radius.z;
		btrCorner.z*= boundingBox.radius.z;

		std::vector<float> vertices = {
			fblCorner.x, fblCorner.y, fblCorner.z,
			ftlCorner.x, ftlCorner.y, ftlCorner.z,

			ftlCorner.x, ftlCorner.y, ftlCorner.z,
			ftrCorner.x, ftrCorner.y, ftrCorner.z,

			ftrCorner.x, ftrCorner.y, ftrCorner.z,
			fbrCorner.x, fbrCorner.y, fbrCorner.z,

			fbrCorner.x, fbrCorner.y, fbrCorner.z,
			fblCorner.x, fblCorner.y, fblCorner.z,

			bblCorner.x, bblCorner.y, bblCorner.z,
			btlCorner.x, btlCorner.y, btlCorner.z,

			btlCorner.x, btlCorner.y, btlCorner.z,
			btrCorner.x, btrCorner.y, btrCorner.z,

			btrCorner.x, btrCorner.y, btrCorner.z,
			bbrCorner.x, bbrCorner.y, bbrCorner.z,

			bbrCorner.x, bbrCorner.y, bbrCorner.z,
			bblCorner.x, bblCorner.y, bblCorner.z,

			btlCorner.x, btlCorner.y, btlCorner.z,
			ftlCorner.x, ftlCorner.y, ftlCorner.z,
			btrCorner.x, btrCorner.y, btrCorner.z,
			ftrCorner.x, ftrCorner.y, ftrCorner.z,
			bblCorner.x, bblCorner.y, bblCorner.z,
			fblCorner.x, fblCorner.y, fblCorner.z,
			bbrCorner.x, bbrCorner.y, bbrCorner.z,
			fbrCorner.x, fbrCorner.y, fbrCorner.z,
		};

		mesh->geometry.addVertex(vertices);

		return mesh;
	}

}
