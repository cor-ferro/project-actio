#ifndef POINT_LIGHT_HELPER_H_
#define POINT_LIGHT_HELPER_H_

#include <vector>
#include <glm/gtc/quaternion.hpp>
#include "../lib/types.h"
#include "../lib/console.h"
#include "../core/model.h"
#include "../core/mesh.h"
#include "../core/vertex.h"
#include "../materials/material.h"
#include "../core/model.h"
#include "../core/mesh.h"
#include "../lights/point_light.h"
#include "../renderer/renderable.h"

namespace Helpers
{
	struct PointLight : Model {
		PointLight(Light::Point * light);
		~PointLight();

		void beforeRender();
		void afterRender();

	protected:
		Mesh * meshHelper;

	private:
		Light::Point * light;
	};
}


#endif /* POINT_LIGHT_HELPER_H_ */
