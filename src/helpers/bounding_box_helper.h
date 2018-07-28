/*
 * bounding_box_helper.h
 *
 *  Created on: Oct 13, 2017
 *      Author: demitriy
 */

#ifndef BOUNDING_BOX_HELPER_H_
#define BOUNDING_BOX_HELPER_H_

#include "../core/model.h"
#include "../core/mesh.h"
#include "../core/vertex.h"
#include "../core/material.h"

namespace Helpers
{
	struct BoundingBox : Model {
		BoundingBox(Model * model);
		~BoundingBox();

	private:
		void determinateBounding();
		Mesh * createBoxMesh(const Math::Box3& boundingBox);
		Model * model_;
	};
}



#endif /* BOUNDING_BOX_HELPER_H_ */
