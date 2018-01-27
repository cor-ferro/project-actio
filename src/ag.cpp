#include "ag.h"

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

		Model * create(ModelConfig& modelConfig)
		{
			return new Model(modelConfig);
		}

		Model * skybox(std::vector<Resource::File> faces)
		{
			Texture texture = Texture::Cube();
			Geometry geometry = Geometry::Box();

			for (char i = 0; i < 6; i++) {
				// @todo: потоковая загрузка
				ImageLoader::Data imageData = ImageLoader::load(faces[i].getPath());
				texture.setData(imageData, i);
			}

			Mesh * mesh = new Mesh(Material::Phong(texture), geometry);
			Model * model = new Model(mesh);

			return model;
		}

		Model * box()
		{
			Material::Phong material;
			material.setWireframe(true);
			material.setDiffuse(0.0f, 1.0f, 0.0f);

			Geometry geometry = Geometry::Box();
			Mesh * mesh = new Mesh(material, geometry);
			Model * model = new Model(mesh);

			return model;
		}

		Model * plane(uint width, uint height, uint widthSegments, uint heightSegments)
		{
			Material::Phong material;
//			material.setWireframe(true);
			material.setDiffuse(0.0f, 1.0f, 0.0f);

			Geometry geometry = Geometry::Plane(width, height, widthSegments, heightSegments);
			Mesh * mesh = new Mesh(material, geometry);
			Model * model = new Model(mesh);

			return model;
		}

		Model * sphere(float radius, uint widthSegments, uint heightSegments)
		{
			Material::Phong material;
//			material.setWireframe(true);
			material.setNoLight(true);
			material.setDiffuse(0.0f, 1.0f, 0.0f);

			Geometry geometry = Geometry::Sphere(radius, widthSegments, heightSegments, 0.0f, glm::two_pi<float>(), 0.0f, 3.14f);
			Mesh * mesh = new Mesh(material, geometry);
			Model * model = new Model(mesh);

			return model;
		}

		Model * circle(float radius, uint segments)
		{
			Material::Phong material;
			material.setWireframe(true);
			material.setDiffuse(0.0f, 1.0f, 0.0f);

			Geometry geometry = Geometry::Circle(radius, segments, 0.0f, glm::two_pi<float>());
			Mesh * mesh = new Mesh(material, geometry);
			Model * model = new Model(mesh);

			return model;
		}

		Model * cylinder(float radiusTop, float radiusBottom, float height, uint radialSegments, uint heightSegments)
		{
			Material::Phong material;
//			material.setWireframe(true);
			material.setDiffuse(0.0f, 1.0f, 0.0f);

			Geometry geometry = Geometry::Cylinder(
				radiusTop, 
				radiusBottom, 
				height, 
				radialSegments, 
				heightSegments, 
				false, 
				0.0f, 
				glm::two_pi<float>()
			);
			Mesh * mesh = new Mesh(material, geometry);
			Model * model = new Model(mesh);

			return model;
		}

		Model * cone(float radius, float height, uint radialSegments, uint heightSegments)
		{
			Material::Phong material;
			material.setWireframe(true);
			material.setDiffuse(0.0f, 1.0f, 0.0f);

			Geometry geometry = Geometry::Cone(radius, height, radialSegments, heightSegments, false, 0.0f, glm::two_pi<float>());
			Mesh * mesh = new Mesh(material, geometry);
			Model * model = new Model(mesh);

			return model;
		}

		Model * ring(float innerRadius, float outerRadius, uint thetaSegments)
		{
			Material::Phong material;
			material.setWireframe(true);
			material.setDiffuse(0.0f, 1.0f, 0.0f);

			Geometry geometry = Geometry::Ring(innerRadius, outerRadius, thetaSegments,	8, 0.0f, glm::two_pi<float>());
			Mesh * mesh = new Mesh(material, geometry);
			Model * model = new Model(mesh);

			return model;
		}

		Model * torus(float radius, float tube, uint radialSegments, uint tubularSegments)
		{
			Material::Phong material;
			material.setDiffuse(0.0f, 1.0f, 0.0f);

			Geometry geometry = Geometry::Torus(radius, tube, radialSegments, tubularSegments, glm::two_pi<float>());
			Mesh * mesh = new Mesh(material, geometry);
			Model * model = new Model(mesh);

			return model;
		}

		Model * octahedron(float radius)
		{
			Material::Phong material;
			material.setWireframe(true);
			material.setDiffuse(0.0f, 1.0f, 0.0f);

			Geometry geometry = Geometry::Octahedron(radius);
			Mesh * mesh = new Mesh(material, geometry);
			Model * model = new Model(mesh);

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
