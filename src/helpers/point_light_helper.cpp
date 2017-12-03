#include "point_light_helper.h"

namespace Helpers
{
    PointLight::PointLight(Light::Point * light)
        : Model()
        , light(light)
    {
        PhongMaterial material;
        material.setWireframe(true);
        material.setDiffuse(0.0f, 1.0f, 0.0f);

        Geometry geometry = Geometry::Octahedron(1.0f);
        meshHelper = new Mesh(material, geometry);
        meshHelper->setup();

        addMesh(meshHelper);
    }

    PointLight::~PointLight() {}

    void PointLight::beforeRender()
    {
        meshHelper->setPosition(light->getPosition());
    }

    void PointLight::afterRender()
    {
        console::info("before render");
    }
}
