#ifndef ACTIO_GEOMETRY_BUILDER_H
#define ACTIO_GEOMETRY_BUILDER_H

#include "geometry.h"

struct GeometryBuilder {
    struct BoxDescription {
        float width = 1.0f;
        float height = 1.0f;
        float depth = 1.0f;
        int widthSegments = 1;
        int heightSegments = 1;
        int depthSegments = 1;
    };

    struct PlaneDescription {
        float width = 1;
        float height = 1;
        uint widthSegments = 1;
        uint heightSegments = 1;
    };

    struct SphereDescription {
        float radius = 1.0f;
        uint widthSegments = 1;
        uint heightSegments = 1;
        float phiStart = 0.0f;
        float phiLength = glm::two_pi<float>();
        float thetaStart = 0.0f;
        float thetaLength = glm::pi<float>();
    };

    struct CircleDescription {
        float radius = 1.0f;
        uint segments = 1;
        float thetaStart = 0.0f;
        float thetaLength = glm::two_pi<float>();
    };

    struct ConeDescription {
        float radius = 1.0f;
        float height = 1.0f;
        uint radialSegments = 1;
        uint heightSegments = 1;
        bool openEnded = true;
        float thetaStart = 0.0f;
        float thetaLength = glm::two_pi<float>();
    };

    struct CylinderCapDescription {
        bool top;
    };

    struct CylinderDescription {
        float radiusTop = 1.0f;
        float radiusBottom = 1.0f;
        float height = 1.0f;
        uint radialSegments = 1;
        uint heightSegments = 1;
        bool openEnded = false;
        float thetaStart = 0.0f;
        float thetaLength = glm::two_pi<float>();
    };

    struct RingDescription {
        float innerRadius = 1.0f;
        float outerRadius = 1.0f;
        uint thetaSegments = 1;
        uint phiSegments = 1;
        float thetaStart = 0.0f;
        float thetaLength = glm::two_pi<float>();
    };

    struct TorusDescription {
        float radius = 1.0f;
        float tube = 1.0f;
        uint radialSegments = 1;
        uint tubularSegments = 1;
        float arc = 1.0f;
    };

    struct OctahedronDescription {
        float radius;
    };

    struct A {
        float x;
    };

    static void Box(Geometry&, const BoxDescription &description);

    static void Plane(Geometry&, const PlaneDescription &description);

    static void Sphere(Geometry&, const SphereDescription &description);

    static void Circle(Geometry&, const CircleDescription& description);

    static void Cone(Geometry&, const ConeDescription &description);

    static void CylinderTorso(Geometry&, GeometryCone& params);

    static void CylinderCap(Geometry&, GeometryCone& params, const CylinderCapDescription &description);

    static void Cylinder(Geometry&, const CylinderDescription &description);

    static void Ring(Geometry&, const RingDescription &description);

    static void Torus(Geometry&, const TorusDescription &description);

    static void Octahedron(Geometry&, const OctahedronDescription &description);

    static void Quad2d(Geometry&);

    static void Lines(Geometry&, std::vector<vec3> lines);

    static void FromAi(Geometry &geometry, const aiMesh *mesh);
};

#endif //ACTIO_GEOMETRY_BUILDER_H
