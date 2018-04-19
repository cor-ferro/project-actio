#include "geometry_primitive.h"

void GeometryPrimitive::Box(
        Geometry &geometry,
        float width,
        float height,
        float depth,
        int widthSegments,
        int heightSegments,
        int depthSegments
) {
    // helper variables
    int numberOfVertices = 0;

    auto buildPlane = [&numberOfVertices, &geometry](
            int u, int v, int w, float udir, float vdir, float width, float height, float depth, int gridX, int gridY
    ) {
        float segmentWidth = width / static_cast<float>(gridX);
        float segmentHeight = height / static_cast<float>(gridY);

        float widthHalf = width / 2.0f;
        float heightHalf = height / 2.0f;
        float depthHalf = depth / 2.0f;

        int gridX1 = static_cast<int>(gridX) + 1;
        int gridY1 = static_cast<int>(gridY) + 1;

        int vertexCounter = 0;
        int ix, iy;

        for (iy = 0; iy < gridY1; iy++) {
            float y = static_cast<float>(iy) * segmentHeight - heightHalf;

            for (ix = 0; ix < gridX1; ix++) {
                float x = static_cast<float>(ix) * segmentWidth - widthHalf;

                vec3 position;
                position[u] = x * udir;
                position[v] = y * vdir;
                position[w] = depthHalf;

                vec3 normal;
                normal[u] = 0.0f;
                normal[v] = 0.0f;
                normal[w] = depth > 0.0f ? 1.0f : -1.0f;

                Vertex vertex;
                vertex.Position = position;
                vertex.Normal = normal;

                geometry.addVertex(vertex);
                vertexCounter += 1;
            }
        }

        for (iy = 0; iy < gridY; iy++) {
            for (ix = 0; ix < gridX; ix++) {
                int a = numberOfVertices + ix + gridX1 * iy;
                int b = numberOfVertices + ix + gridX1 * (iy + 1);
                int c = numberOfVertices + (ix + 1) + gridX1 * (iy + 1);
                int d = numberOfVertices + (ix + 1) + gridX1 * iy;

                geometry.addFace(a, b, d);
                geometry.addFace(b, c, d);
            }
        }

        numberOfVertices += vertexCounter;
    };

#define x_axis 0
#define y_axis 1
#define z_axis 2

    buildPlane(z_axis, y_axis, x_axis, -1.0f, -1.0f, depth, height, width, depthSegments, heightSegments);
    buildPlane(z_axis, y_axis, x_axis, 1.0f, -1.0f, depth, height, -width, depthSegments, heightSegments);
    buildPlane(x_axis, z_axis, y_axis, 1.0f, 1.0f, width, depth, height, widthSegments, depthSegments);
    buildPlane(x_axis, z_axis, y_axis, 1.0f, -1.0f, width, depth, -height, widthSegments, depthSegments);
    buildPlane(x_axis, y_axis, z_axis, 1.0f, -1.0f, width, height, depth, widthSegments, heightSegments);
    buildPlane(x_axis, y_axis, z_axis, -1.0f, -1.0f, width, height, -depth, widthSegments, heightSegments);

#undef x_axis
#undef y_axis
#undef z_axis

}

void GeometryPrimitive::Plane(
        Geometry &geometry,
        uint width,
        uint height,
        uint widthSegments,
        uint heightSegments
) {
    float width_half = (float) width / 2.0f;
    float height_half = (float) height / 2.0f;

    uint gridX = widthSegments;
    uint gridY = heightSegments;

    uint gridX1 = gridX + 1;
    uint gridY1 = gridY + 1;

    float segment_width = (float) width / (float) gridX;
    float segment_height = (float) height / (float) gridY;

    geometry.allocVertices(gridY1 * gridX1);
    geometry.allocIndices(gridY * gridX);

    for (uint iy = 0; iy < gridY1; iy++) {
        float y = (float) iy * segment_height - height_half;

        for (uint ix = 0; ix < gridX1; ix++) {
            float x = (float) ix * segment_width - width_half;

            Vertex vertex;
            vertex.Position = vec3(x, -y, 0.0f);
            vertex.Normal = vec3(0.0f, 0.0f, -1.0f);

            geometry.addVertex(vertex);
        }
    }

    for (uint iy = 0; iy < gridY; iy++) {
        for (uint ix = 0; ix < gridX; ix++) {
            uint a = ix + gridX1 * iy;
            uint b = ix + gridX1 * (iy + 1);
            uint c = (ix + 1) + gridX1 * (iy + 1);
            uint d = (ix + 1) + gridX1 * iy;

            geometry.addFace(a, b, d);
            geometry.addFace(b, c, d);
        }
    }
}

void GeometryPrimitive::Sphere(
        Geometry &geometry,
        float radius,
        uint widthSegments,
        uint heightSegments,
        float phiStart,
        float phiLength,
        float thetaStart,
        float thetaLength
) {
    uint minWidthSegments = 3;
    uint minHeightSegments = 2;

    widthSegments = glm::max(minWidthSegments, widthSegments);
    heightSegments = glm::max(minHeightSegments, heightSegments);

    float thetaEnd = thetaStart + thetaLength;

    uint index = 0;
    std::vector<std::vector<uint>> grid;

    for (uint iy = 0; iy <= heightSegments; iy++) {
        std::vector<uint> verticesRow;

        float v = (float) iy / (float) heightSegments;
        for (uint ix = 0; ix <= widthSegments; ix++) {
            float u = (float) ix / (float) widthSegments;

            Vertex vertex;

            vertex.Position = vec3(
                    -radius * glm::cos(phiStart + u * phiLength) * glm::sin(thetaStart + v * thetaLength),
                    radius * glm::cos(thetaStart + v * thetaLength),
                    radius * glm::sin(phiStart + u * phiLength) * glm::sin(thetaStart + v * thetaLength)
            );

            vertex.Normal = glm::normalize(vec3(
                    vertex.Position.x,
                    vertex.Position.y,
                    vertex.Position.z
            ));

            geometry.addVertex(vertex);

            verticesRow.push_back(index++);
        }

        grid.push_back(verticesRow);
    }

    // indices
    for (uint iy = 0; iy < heightSegments; iy++) {
        for (uint ix = 0; ix < widthSegments; ix++) {
            uint a = grid[iy][ix + 1];
            uint b = grid[iy][ix];
            uint c = grid[iy + 1][ix];
            uint d = grid[iy + 1][ix + 1];

            if (iy != 0 || thetaStart > 0) {
                geometry.addFace(a, b, d);
            }

            if (iy != heightSegments - 1 || thetaEnd < glm::pi<float>()) {
                geometry.addFace(b, c, d);
            }
        }
    }
}

void GeometryPrimitive::Circle(
        Geometry &geometry,
        float radius,
        uint segments,
        float thetaStart,
        float thetaLength
) {
    uint minSegments = 3;
    segments = glm::max(minSegments, segments);

    /*thetaStart = thetaStart !== undefined ? thetaStart : 0;
    thetaLength = thetaLength !== undefined ? thetaLength : Math.PI * 2;*/

    Vertex centerVertex;
    centerVertex.Position = vec3(0.0f, 0.0f, 0.0f);
    centerVertex.Normal = vec3(0.0f, 0.0f, 1.0f);

    geometry.addVertex(centerVertex);

    for (uint s = 0, i = 3; s <= segments; s++, i += 3) {
        float segment = thetaStart + (float) s / (float) segments * thetaLength;

        Vertex vertex;
        vertex.Position = vec3(
                radius * glm::cos(segment),
                radius * glm::sin(segment),
                0.0f
        );
        vertex.Normal = vec3(
                0.0f,
                0.0f,
                1.0f
        );

        geometry.addVertex(vertex);
    }

    for (uint i = 1; i <= segments; i++) {
        geometry.addFace(i, i + 1, 0);
    }
}

void GeometryPrimitive::CylinderTorso(
        Geometry &geometry,
        GeometryCone &params
) {
    std::vector<std::vector<uint>> indexArray;
    float halfHeight = params.height / 2.0f;
    float slope = (params.radiusBottom - params.radiusTop) / params.height;

    uint index = geometry.getCountVertices();

    for (uint y = 0; y <= params.heightSegments; y++) {
        std::vector<uint> indexRow;

        float v = (float) y / (float) params.heightSegments;
        float radius = v * (params.radiusBottom - params.radiusTop) + params.radiusTop;

        for (uint x = 0; x <= params.radialSegments; x++) {
            float u = (float) x / (float) params.radialSegments;
            float theta = u * params.thetaLength + params.thetaStart;

            float sinTheta = glm::sin(theta);
            float cosTheta = glm::cos(theta);

            // vertex
            Vertex vertex;
            vertex.Position = vec3(
                    radius * sinTheta,
                    -v * params.height + halfHeight,
                    radius * cosTheta
            );
            vertex.Normal = glm::normalize(vec3(
                    sinTheta,
                    slope,
                    cosTheta
            ));

            geometry.addVertex(vertex);
            indexRow.push_back(index++);
        }

        indexArray.push_back(indexRow);
    }

    for (uint x = 0; x < params.radialSegments; x++) {
        for (uint y = 0; y < params.heightSegments; y++) {
            uint a = indexArray[y][x];
            uint b = indexArray[y + 1][x];
            uint c = indexArray[y + 1][x + 1];
            uint d = indexArray[y][x + 1];

            geometry.addFace(a, b, d);
            geometry.addFace(b, c, d);
        }
    }
}

void GeometryPrimitive::CylinderCap(
        Geometry &geometry,
        GeometryCone &params,
        bool top
) {
    uint centerIndexStart, centerIndexEnd;

    float halfHeight = params.height / 2.0f;
    float radius = (top == true) ? params.radiusTop : params.radiusBottom;
    float sign = (top == true) ? 1.0 : -1.0;

    uint index = geometry.getCountVertices();

    centerIndexStart = index;

    for (uint x = 1; x <= params.radialSegments; x++) {
        Vertex vertex;
        vertex.Position = vec3(0.0f, halfHeight * sign, 0.0f);
        vertex.Normal = vec3(0.0f, sign, 0.0f);

        geometry.addVertex(vertex);

        index++;
    }

    centerIndexEnd = index;

    for (uint x = 0; x <= params.radialSegments; x++) {
        float u = (float) x / (float) params.radialSegments;
        float theta = u * params.thetaLength + params.thetaStart;

        float cosTheta = glm::cos(theta);
        float sinTheta = glm::sin(theta);

        Vertex vertex;
        vertex.Position = vec3(radius * sinTheta, halfHeight * sign, radius * cosTheta);
        vertex.Normal = vec3(0.0f, sign, 0.0f);

        geometry.addVertex(vertex);

        index++;
    }

    for (uint x = 0; x < params.radialSegments; x++) {
        uint c = centerIndexStart + x;
        uint i = centerIndexEnd + x;

        if (top == true) {
            geometry.addFace(i, i + 1, c);
        } else {
            geometry.addFace(i + 1, i, c);
        }
    }
}

void GeometryPrimitive::Cylinder(
        Geometry &geometry,
        float radiusTop,
        float radiusBottom,
        float height,
        uint radialSegments,
        uint heightSegments,
        bool openEnded,
        float thetaStart,
        float thetaLength
) {
    GeometryCone params = {
            .radiusTop = radiusTop,
            .radiusBottom = radiusBottom,
            .height = height,
            .radialSegments = radialSegments,
            .heightSegments = heightSegments,
            .openEnded = openEnded,
            .thetaStart = thetaStart,
            .thetaLength = thetaLength
    };

    CylinderTorso(geometry, params);

    if (!openEnded) {
        if (radiusTop > 0) CylinderCap(geometry, params, true);
        if (radiusBottom > 0) CylinderCap(geometry, params, false);
    }
}

void GeometryPrimitive::Cone(
        Geometry &geometry,
        float radius,
        float height,
        uint radialSegments,
        uint heightSegments,
        bool openEnded,
        float thetaStart,
        float thetaLength
) {
    GeometryPrimitive::Cylinder(geometry, 0.0f, radius, height, radialSegments, heightSegments, openEnded, thetaStart,
                                thetaLength);
}

void GeometryPrimitive::Ring(
        Geometry &geometry,
        float innerRadius,
        float outerRadius,
        uint thetaSegments,
        uint phiSegments,
        float thetaStart,
        float thetaLength
) {
    float radius = innerRadius;
    float radiusStep = ((outerRadius - innerRadius) / (float) phiSegments);

    for (uint j = 0; j <= phiSegments; j++) {
        for (uint i = 0; i <= thetaSegments; i++) {
            float segment = thetaStart + (float) i / (float) thetaSegments * thetaLength;

            Vertex vertex;
            vertex.Position = vec3(
                    radius * glm::cos(segment),
                    radius * glm::sin(segment),
                    0.0f
            );
            vertex.Normal = vec3(
                    0.0f,
                    0.0f,
                    1.0f
            );

            geometry.addVertex(vertex);
        }

        radius += radiusStep;
    }

    for (uint j = 0; j < phiSegments; j++) {
        uint thetaSegmentLevel = j * (thetaSegments + 1);
        for (uint i = 0; i < thetaSegments; i++) {
            uint segment = i + thetaSegmentLevel;

            uint a = segment;
            uint b = segment + thetaSegments + 1;
            uint c = segment + thetaSegments + 2;
            uint d = segment + 1;

            geometry.addFace(a, b, d);
            geometry.addFace(b, c, d);
        }
    }
}

void GeometryPrimitive::Torus(
        Geometry &geometry,
        float radius,
        float tube,
        uint radialSegments,
        uint tubularSegments,
        float arc
) {
    // generate vertices, normals and uvs
    for (uint j = 0; j <= radialSegments; j++) {
        for (uint i = 0; i <= tubularSegments; i++) {

            float u = (float) i / (float) tubularSegments * arc;
            float v = (float) j / (float) radialSegments * glm::two_pi<float>();

            Vertex vertex;
            vertex.Position = vec3(
                    (radius + tube * glm::cos(v)) * glm::cos(u),
                    (radius + tube * glm::cos(v)) * glm::sin(u),
                    tube * glm::sin(v)
            );

            vec3 center(
                    radius * glm::cos(u),
                    radius * glm::sin(u),
                    0.0f
            );

            vertex.Normal = glm::normalize(center - vertex.Position);

            geometry.addVertex(vertex);
        }
    }

    for (uint j = 1; j <= radialSegments; j++) {
        for (uint i = 1; i <= tubularSegments; i++) {
            uint a = (tubularSegments + 1) * j + i - 1;
            uint b = (tubularSegments + 1) * (j - 1) + i - 1;
            uint c = (tubularSegments + 1) * (j - 1) + i;
            uint d = (tubularSegments + 1) * j + i;

            geometry.addFace(a, b, d);
            geometry.addFace(b, c, d);
        }
    }
}

void GeometryPrimitive::Octahedron(Geometry &geometry, float radius) {
    float vertices[] = {
            1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, -1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, -1.0f
    };

    uint indices[] = {
            0, 2, 4,
            0, 4, 3,
            0, 3, 5,
            0, 5, 2,
            1, 2, 5,
            1, 5, 3,
            1, 3, 4,
            1, 4, 2
    };

    uint countVertices = 18;
    uint countIndices = 24;

    for (uint i = 0; i < countVertices; i += 3) {
        Vertex vertex;
        vertex.Position = vec3(vertices[i] * radius, vertices[i + 1] * radius, vertices[i + 2] * radius);
        vertex.Normal = glm::normalize(vec3(vertex.Position.x, vertex.Position.y, vertex.Position.z));

        geometry.addVertex(vertex);
    }

    for (uint i = 0; i < countIndices; i += 3) {
        geometry.addFace(indices[i], indices[i + 1], indices[i + 2]);
    }
}

void GeometryPrimitive::Quad2d(Geometry &geometry) {
    float vertices[] = {
            1.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f
    };

    uint indices[] = {
            0, 1, 3,
            1, 2, 3
    };

    uint countVertices = 12;
    uint countIndices = 6;

    for (uint i = 0; i < countVertices; i += 3) {
        Vertex vertex;
        vertex.Position = vec3(vertices[i], vertices[i + 1], vertices[i + 2]);
        vertex.Normal = glm::normalize(vec3(vertex.Position.x, vertex.Position.y, vertex.Position.z));

        geometry.addVertex(vertex);
    }

    for (uint i = 0; i < countIndices; i += 3) {
        geometry.addFace(indices[i], indices[i + 1], indices[i + 2]);
    }
}

void GeometryPrimitive::Lines(Geometry &geometry, std::vector<vec3> lines) {
    assert(lines.size() % 2 == 0);

    for (vec3 &point : lines) {
        geometry.addVertex(point);
    }
}