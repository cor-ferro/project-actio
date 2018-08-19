#ifndef ACTIO_GEOMETRY_BUILDER_H
#define ACTIO_GEOMETRY_BUILDER_H

#include "geometry.h"

struct GeometryBuilder {
    static void Box(Geometry&, float width = 1.0f, float height = 1.0f, float depth = 1.0f, int widthSegments = 1, int heightSegments = 1, int depthSegments = 1);

    static void Plane(Geometry&, uint width, uint height, uint widthSegments, uint heightSegments);

    static void Sphere(Geometry&, float radius, uint widthSegments, uint heightSegments, float phiStart, float phiLength, float thetaStart, float thetaLength);

    static void Circle(Geometry&, float radius, uint segments, float thetaStart, float thetaLength);

    static void Cone(Geometry&, float radius, float height, uint radialSegments, uint heightSegments, bool openEnded, float thetaStart, float thetaLength);

    static void CylinderTorso(Geometry&, GeometryCone& params);

    static void CylinderCap(Geometry&, GeometryCone& params, bool top);

    static void Cylinder(Geometry&, float radiusTop, float radiusBottom, float height, uint radialSegments, uint heightSegments, bool openEnded, float thetaStart, float thetaLength);

    static void Ring(Geometry&, float innerRadius, float outerRadius, uint thetaSegments, uint phiSegments, float thetaStart, float thetaLength);

    static void Torus(Geometry&, float radius, float tube, uint radialSegments, uint tubularSegments, float arc);

    static void Octahedron(Geometry&, float radius);

    static void Quad2d(Geometry&);

    static void Lines(Geometry&, std::vector<vec3> lines);

    static void FromAi(Geometry &geometry, const aiMesh *mesh);
};

#endif //ACTIO_GEOMETRY_BUILDER_H
