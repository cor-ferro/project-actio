#include "geometry_builder.h"
#include "../lib/assimp.h"

#define MAX_BONES 4

void GeometryBuilder::Box(Geometry &geometry, const BoxDescription &desc) {
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

                vec2 texCoord = vec2(static_cast<float>(ix) / static_cast<float>(gridX), 1.0f - (static_cast<float>(iy) / static_cast<float>(gridY)));

                Vertex vertex;
                vertex.Position = position;
                vertex.Normal = normal;
                vertex.TexCoords = texCoord;

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

    buildPlane(z_axis, y_axis, x_axis, -1.0f, -1.0f, desc.depth, desc.height, desc.width, desc.depthSegments, desc.heightSegments);
    buildPlane(z_axis, y_axis, x_axis, 1.0f, -1.0f, desc.depth, desc.height, -desc.width, desc.depthSegments, desc.heightSegments);
    buildPlane(x_axis, z_axis, y_axis, 1.0f, 1.0f, desc.width, desc.depth, desc.height, desc.widthSegments, desc.depthSegments);
    buildPlane(x_axis, z_axis, y_axis, 1.0f, -1.0f, desc.width, desc.depth, -desc.height, desc.widthSegments, desc.depthSegments);
    buildPlane(x_axis, y_axis, z_axis, 1.0f, -1.0f, desc.width, desc.height, desc.depth, desc.widthSegments, desc.heightSegments);
    buildPlane(x_axis, y_axis, z_axis, -1.0f, -1.0f, desc.width, desc.height, -desc.depth, desc.widthSegments, desc.heightSegments);

#undef x_axis
#undef y_axis
#undef z_axis

    geometry.computeBoundingBox();

}

void GeometryBuilder::Plane(Geometry &geometry, const PlaneDescription &desc) {
    float width_half = (float) desc.width / 2.0f;
    float height_half = (float) desc.height / 2.0f;

    uint gridX = desc.widthSegments;
    uint gridY = desc.heightSegments;

    uint gridX1 = gridX + 1;
    uint gridY1 = gridY + 1;

    float segment_width = (float) desc.width / (float) gridX;
    float segment_height = (float) desc.height / (float) gridY;

    geometry.allocVertices(gridY1 * gridX1);
    geometry.allocIndices(gridY * gridX);

    for (uint iy = 0; iy < gridY1; iy++) {
        float y = (float) iy * segment_height - height_half;

        for (uint ix = 0; ix < gridX1; ix++) {
            float x = (float) ix * segment_width - width_half;

            Vertex vertex;
            vertex.Position = vec3(x, -y, 0.0f);
            vertex.Normal = vec3(0.0f, 0.0f, -1.0f);
            vertex.TexCoords = vec2(static_cast<float>(ix) / static_cast<float>(gridX), 1.0f - (static_cast<float>(iy) / static_cast<float>(gridY)));

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

    geometry.computeBoundingBox();
}

void GeometryBuilder::Sphere(Geometry &geometry, const SphereDescription &desc) {
    uint minWidthSegments = 3;
    uint minHeightSegments = 2;

    uint widthSegments = glm::max(minWidthSegments, desc.widthSegments);
    uint heightSegments = glm::max(minHeightSegments, desc.heightSegments);

    float thetaEnd = desc.thetaStart + desc.thetaLength;

    uint index = 0;
    std::vector<std::vector<uint>> grid;

    for (uint iy = 0; iy <= heightSegments; iy++) {
        std::vector<uint> verticesRow;

        float v = (float) iy / (float) heightSegments;
        for (uint ix = 0; ix <= widthSegments; ix++) {
            float u = (float) ix / (float) widthSegments;

            Vertex vertex;

            vertex.Position = vec3(
                    -desc.radius * glm::cos(desc.phiStart + u * desc.phiLength) * glm::sin(desc.thetaStart + v * desc.thetaLength),
                    desc.radius * glm::cos(desc.thetaStart + v * desc.thetaLength),
                    desc.radius * glm::sin(desc.phiStart + u * desc.phiLength) * glm::sin(desc.thetaStart + v * desc.thetaLength)
            );

            vertex.Normal = glm::normalize(vec3(
                    vertex.Position.x,
                    vertex.Position.y,
                    vertex.Position.z
            ));

            vertex.TexCoords = vec2(u, 1 - v);

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

            if (iy != 0 || desc.thetaStart > 0) {
                geometry.addFace(a, b, d);
            }

            if (iy != heightSegments - 1 || thetaEnd < glm::pi<float>()) {
                geometry.addFace(b, c, d);
            }
        }
    }

    geometry.computeBoundingBox();
}

void GeometryBuilder::Circle(Geometry &geometry, const CircleDescription& desc) {
    uint minSegments = 3;
    uint segments = glm::max(minSegments, desc.segments);

    /*thetaStart = thetaStart !== undefined ? thetaStart : 0;
    thetaLength = thetaLength !== undefined ? thetaLength : Math.PI * 2;*/

    Vertex centerVertex;
    centerVertex.Position = vec3(0.0f, 0.0f, 0.0f);
    centerVertex.Normal = vec3(0.0f, 0.0f, 1.0f);

    geometry.addVertex(centerVertex);

    for (uint s = 0, i = 3; s <= segments; s++, i += 3) {
        float segment = desc.thetaStart + (float) s / (float) segments * desc.thetaLength;

        Vertex vertex;
        vertex.Position = vec3(
                desc.radius * glm::cos(segment),
                desc.radius * glm::sin(segment),
                0.0f
        );
        vertex.Normal = vec3(
                0.0f,
                0.0f,
                1.0f
        );

        vertex.TexCoords = vec2(
             (vertex.Position.x / desc.radius + 1.0f) * 0.5f,
             (vertex.Position.y / desc.radius + 1.0f) * 0.5f
        );

        geometry.addVertex(vertex);
    }

    for (uint i = 1; i <= segments; i++) {
        geometry.addFace(i, i + 1, 0);
    }

    geometry.computeBoundingBox();
}

void GeometryBuilder::CylinderTorso(Geometry &geometry, GeometryCone &params) {
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

            vertex.TexCoords = vec2(u, 1 - v);

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

    geometry.computeBoundingBox();
}

void GeometryBuilder::CylinderCap(Geometry &geometry, GeometryCone &params, const CylinderCapDescription &desc) {
    uint centerIndexStart, centerIndexEnd;

    float halfHeight = params.height / 2.0f;
    float radius = (desc.top == true) ? params.radiusTop : params.radiusBottom;
    float sign = (desc.top == true) ? 1.0 : -1.0;

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
        vertex.TexCoords = vec2(0.5f, 0.5f);

        geometry.addVertex(vertex);

        index++;
    }

    for (uint x = 0; x < params.radialSegments; x++) {
        uint c = centerIndexStart + x;
        uint i = centerIndexEnd + x;

        if (desc.top == true) {
            geometry.addFace(i, i + 1, c);
        } else {
            geometry.addFace(i + 1, i, c);
        }
    }

    geometry.computeBoundingBox();
}

void GeometryBuilder::Cylinder(Geometry &geometry, const CylinderDescription &desc) {
    GeometryCone params = {
            .radiusTop = desc.radiusTop,
            .radiusBottom = desc.radiusBottom,
            .height = desc.height,
            .radialSegments = desc.radialSegments,
            .heightSegments = desc.heightSegments,
            .openEnded = desc.openEnded,
            .thetaStart = desc.thetaStart,
            .thetaLength = desc.thetaLength
    };

    CylinderTorso(geometry, params);

    if (!desc.openEnded) {
        if (desc.radiusTop > 0) CylinderCap(geometry, params, {.top = true});
        if (desc.radiusBottom > 0) CylinderCap(geometry, params, {.top = false});
    }

    geometry.computeBoundingBox();
}

void GeometryBuilder::Cone(Geometry &geometry, const ConeDescription &desc) {
    CylinderDescription cylinderDescription;
    cylinderDescription.radiusTop = 0.0f;
    cylinderDescription.radiusBottom = desc.radius;
    cylinderDescription.height = desc.height;
    cylinderDescription.radialSegments = desc.radialSegments;
    cylinderDescription.heightSegments = desc.heightSegments;
    cylinderDescription.openEnded = desc.openEnded;
    cylinderDescription.thetaStart = desc.thetaStart;
    cylinderDescription.thetaLength = desc.thetaLength;

    GeometryBuilder::Cylinder(geometry, cylinderDescription);
}

void GeometryBuilder::Ring(Geometry &geometry, const RingDescription &desc) {
    float radius = desc.innerRadius;
    float radiusStep = ((desc.outerRadius - desc.innerRadius) / (float) desc.phiSegments);

    for (uint j = 0; j <= desc.phiSegments; j++) {
        for (uint i = 0; i <= desc.thetaSegments; i++) {
            float segment = desc.thetaStart + (float) i / (float) desc.thetaSegments * desc.thetaLength;

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
            vertex.TexCoords = vec2((vertex.Position.x / desc.outerRadius + 1.0f) / 2.0f, (vertex.Position.y / desc.outerRadius + 1.0f) / 2.0f);

            geometry.addVertex(vertex);
        }

        radius += radiusStep;
    }

    for (uint j = 0; j < desc.phiSegments; j++) {
        uint thetaSegmentLevel = j * (desc.thetaSegments + 1);
        for (uint i = 0; i < desc.thetaSegments; i++) {
            uint segment = i + thetaSegmentLevel;

            uint a = segment;
            uint b = segment + desc.thetaSegments + 1;
            uint c = segment + desc.thetaSegments + 2;
            uint d = segment + 1;

            geometry.addFace(a, b, d);
            geometry.addFace(b, c, d);
        }
    }

    geometry.computeBoundingBox();
}

void GeometryBuilder::Torus(Geometry &geometry, const TorusDescription &desc) {
    // generate vertices, normals and uvs
    for (uint j = 0; j <= desc.radialSegments; j++) {
        for (uint i = 0; i <= desc.tubularSegments; i++) {

            float u = (float) i / (float) desc.tubularSegments * desc.arc;
            float v = (float) j / (float) desc.radialSegments * glm::two_pi<float>();

            Vertex vertex;
            vertex.Position = vec3(
                    (desc.radius + desc.tube * glm::cos(v)) * glm::cos(u),
                    (desc.radius + desc.tube * glm::cos(v)) * glm::sin(u),
                    desc.tube * glm::sin(v)
            );

            vec3 center(
                    desc.radius * glm::cos(u),
                    desc.radius * glm::sin(u),
                    0.0f
            );

            vertex.Normal = glm::normalize(center - vertex.Position);

            vertex.TexCoords = vec2(
                static_cast<float>(i) / static_cast<float>(desc.tubularSegments),
                static_cast<float>(j) / static_cast<float>(desc.radialSegments)
            );

            geometry.addVertex(vertex);
        }
    }

    for (uint j = 1; j <= desc.radialSegments; j++) {
        for (uint i = 1; i <= desc.tubularSegments; i++) {
            uint a = (desc.tubularSegments + 1) * j + i - 1;
            uint b = (desc.tubularSegments + 1) * (j - 1) + i - 1;
            uint c = (desc.tubularSegments + 1) * (j - 1) + i;
            uint d = (desc.tubularSegments + 1) * j + i;

            geometry.addFace(a, b, d);
            geometry.addFace(b, c, d);
        }
    }

    geometry.computeBoundingBox();
}

void GeometryBuilder::Octahedron(Geometry &geometry, const OctahedronDescription &desc) {
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
        vertex.Position = vec3(vertices[i] * desc.radius, vertices[i + 1] * desc.radius, vertices[i + 2] * desc.radius);
        vertex.Normal = glm::normalize(vec3(vertex.Position.x, vertex.Position.y, vertex.Position.z));

        geometry.addVertex(vertex);
    }

    for (uint i = 0; i < countIndices; i += 3) {
        geometry.addFace(indices[i], indices[i + 1], indices[i + 2]);
    }

    geometry.computeBoundingBox();
}

void GeometryBuilder::Quad2d(Geometry &geometry) {
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

    geometry.computeBoundingBox();
}

void GeometryBuilder::Lines(Geometry &geometry, std::vector<vec3> lines) {
    assert(lines.size() % 2 == 0);

    for (vec3 &point : lines) {
        geometry.addVertex(point);
    }

    geometry.computeBoundingBox();
}

void GeometryBuilder::FromAi(Geometry &geometry, const aiMesh *mesh) {
    unsigned int numVertices = mesh->mNumVertices;
    unsigned int numBones = mesh->mNumBones;

    geometry.allocVertices(numVertices);

    for (unsigned int i = 0; i < numVertices; i++) {
        Vertex vertex;

        vertex.Position = libAi::toNativeType(mesh->mVertices[i]);

        if (mesh->mNormals != nullptr) {
            vertex.Normal = libAi::toNativeType(mesh->mNormals[i]);
        } else {
            vertex.Normal = vec3(0.0f, 1.0f, 0.0f);
        }

        if (mesh->mTextureCoords[0]) {
            vertex.TexCoords = vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        } else {
            vertex.TexCoords = vec2(0.0f, 0.0f);
        }

        if (mesh->mTangents != nullptr) {
            vertex.Tangent = libAi::toNativeType(mesh->mTangents[i]);
            vertex.Bitangent = libAi::toNativeType(mesh->mBitangents[i]);
        } else {
            vertex.Tangent = vec3(1.0f);
            vertex.Bitangent = vec3(1.0f);
        }

        geometry.addVertex(vertex);
    }

    if (numBones > 0) {
        std::vector<std::vector<std::pair<uint, float>>> affectedVertices;
        affectedVertices.resize(geometry.getCountVertices());

        for (uint i = 0; i < numBones; i++) {
            aiBone *bone = mesh->mBones[i];

            for (uint j = 0; j < bone->mNumWeights; j++) {
                uint vertexId = bone->mWeights[j].mVertexId;
                float weight = bone->mWeights[j].mWeight;

                affectedVertices.at(vertexId).push_back({i, weight});
            }
        }

        for (uint i = 0; i < affectedVertices.size(); i++) {
            const std::vector<std::pair<uint, float>> &boneData = affectedVertices.at(i);
            const unsigned long bonesDataSize = boneData.size();

            for (uint j = 0; j < MAX_BONES; j++) {
                if (j < bonesDataSize) {
                    geometry.getVertex(i).BonedIDs[j] = boneData[j].first;
                    geometry.getVertex(i).Weights[j] = boneData[j].second;
                } else {
                    geometry.getVertex(i).BonedIDs[j] = 0;
                    geometry.getVertex(i).Weights[j] = 0;
                }
            }
        }
    }

    unsigned int numFaces = mesh->mNumFaces;

    geometry.freeIndices();
    geometry.allocIndices(numFaces);

    for (unsigned int i = 0; i < numFaces; i++) {
        aiFace &face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            geometry.addIndex(face.mIndices[j]);
        }
    }
}