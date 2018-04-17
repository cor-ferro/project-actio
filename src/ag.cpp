#include "ag.h"
#include "core/geometry_primitive.h"

namespace AG
{
	PerspectiveCamera * createPerspectiveCamera(float fov, float aspect, float near, float far)
	{
		return new PerspectiveCamera(fov, aspect, near, far);
	}

	OrthographicCamera * createOrthographicCamera(float left, float right, float bottom, float top, float near, float far)
	{
		return new OrthographicCamera(left, right, bottom, top, near, far);
	}

	namespace Models {
		// Model * create(Resource::File fileResource) {
		// 	return new Model(fileResource);
		// }

		Model * create(Model::Config& modelConfig)
		{
			return Model::Create(modelConfig);
		}

		Model * skybox(std::vector<Resource::File> faces)
		{
			Texture texture = Texture::Cube();

			for (char i = 0; i < 6; i++) {
				// @todo: потоковая загрузка
				ImageLoader::Data imageData = ImageLoader::load(faces[i].getPath());
				texture.setData(imageData, i);
			}

			Mesh * mesh = Mesh::Create();

			GeometryPrimitive::Box(mesh->geometry);
            mesh->material.addTexture(texture);

			Model * model = Model::Create(mesh);

			return model;
		}

	}

	namespace Helper {
		HelperArrow * arrow(vec3 origin, vec3 direction, float scale)
		{
			return new Helpers::Arrow(origin, direction, scale);
		}

		HelperCameraOrientation * cameraOrientation(vec3 origin, vec3 direction, float length)
		{
			return new Helpers::CameraOrientation(origin, direction, length);
		}
	}

	namespace Light {
		::Light::Directional * directional()
		{
			return new ::Light::Directional(vec3(1.0f), vec3(1.0f), vec3(1.0f));
		}

		::Light::Point * point()
		{
			return new ::Light::Point(vec3(1.0f), vec3(1.0f), vec3(1.0f), vec3(1.0f));
		}

		::Light::Spot * spot()
		{
			return new ::Light::Spot();
		}

		::Light::Directional * directional(vec3 ambient, vec3 diffuse, vec3 specular)
		{
			return new ::Light::Directional(ambient, diffuse, specular);
		}

		::Light::Point * point(vec3 position, vec3 ambient, vec3 diffuse, vec3 specular)
		{
			return new ::Light::Point(position, ambient, diffuse, specular);
		}

		::Light::Spot * spot(vec3 ambient, vec3 diffuse, vec3 specular)
		{
			return new ::Light::Spot(ambient, diffuse, specular);
		}
	}
}
