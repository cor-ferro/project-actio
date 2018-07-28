#include "point_light_helper.h"
#include "../core/geometry_primitive.h"

namespace Helpers
{
    PointLight::PointLight(Light::Point * light)
        : Model()
        , light(light)
    {
        meshHelper = Mesh::Create();

        GeometryPrimitive::Octahedron(meshHelper->geometry, 1.0f);
        meshHelper->material->setWireframe(true);
        meshHelper->material->setDiffuse(0.0f, 1.0f, 0.0f);

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
