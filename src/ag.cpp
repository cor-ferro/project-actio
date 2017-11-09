#include "ag.h"

namespace AG
{
	Scene * createScene()
	{
		return new Scene();
	}

	PerspectiveCamera * createPerspectiveCamera(float fov, float aspect, float near, float far)
	{
		return new PerspectiveCamera(fov, aspect, near, far);
	}

	namespace Models {
		Model * create(Resource::File fileResource) {
			return new Model(fileResource);
		}

		Model * skybox(std::vector<Resource::File> faces) {
			Texture texture = Texture::Cube();
			Geometry geometry = Geometry::Box();

			for (char i = 0; i < 6; i++) {
				// @todo: переделать на потоки
				ImageLoader::Data imageData = ImageLoader::load(faces[i].getPath());
				texture.setData(imageData, i);
			}

			Mesh * mesh = new Mesh(PhongMaterial(texture), geometry);
			Model * model = new Model(mesh);

			return model;
		}
	}

	namespace Helper {
		HelperArrow * arrow(vec3 origin, vec3 direction, float scale) {
			return new RenderHelpers::Arrow(origin, direction, scale);
		}

		HelperCameraOrientation * cameraOrientation(vec3 origin, vec3 direction, float length) {
			return new RenderHelpers::CameraOrientation(origin, direction, length);
		}
	}

	namespace Light {
		LightDirectional * directional(vec3 ambient, vec3 diffuse, vec3 specular) {
			return new Renderer::Light::Directional(ambient, diffuse, specular);
		}

		LightPoint * point(vec3 ambient, vec3 diffuse, vec3 specular) {
			return new Renderer::Light::Point(ambient, diffuse, specular);
		}

		LightSpot * spot(vec3 ambient, vec3 diffuse, vec3 specular) {
			return new Renderer::Light::Spot(ambient, diffuse, specular);
		}
	}
}
