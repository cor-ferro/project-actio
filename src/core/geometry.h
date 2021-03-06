#ifndef ACTIO_CORE_GEOMETRY_H_
#define ACTIO_CORE_GEOMETRY_H_

#include <glad/glad.h>

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtx/normal.hpp>
#include <glm/gtc/constants.hpp>
#include "../lib/types.h"
#include "../lib/console.h"
#include "../resources/resources.h"
#include "../math/Box3.h"
#include "../math/Sphere.h"
#include "../renderer/handle.h"
#include "vertex.h"

using MeshIndex = unsigned int;
using GeometryVertices = std::vector<Vertex>;
using GeometryIndices = std::vector<MeshIndex>;

struct GeometryCone {
    float radiusTop;
    float radiusBottom;
    float height;
    uint radialSegments;
    uint heightSegments;
    bool openEnded;
    float thetaStart;
    float thetaLength;
};

struct Geometry {
    enum GeometryType {
        Geometry_Static = 1,
        Geometry_Dynamic = 2,
    };

    Geometry();

    Geometry(const Geometry &geometry);

    ~Geometry() = default;

    void destroy();

    static int VertexSize() { return sizeof(Vertex); }

    GeometryVertices *getVertices();

    GeometryIndices *getIndices();

    void setType(GeometryType newType);

    const GeometryType& getType() const;

    void setVertices(const std::vector<vec3>& vertices);

    void addVertex(const Vertex &vertex);

    void addVertex(const Vertex &&vertex);

    void addVertex(float x, float y, float z);

    void addVertex(std::vector<float> &vertices);

    Vertex &getVertex(unsigned int index);

    void fillIndices();

    void addIndex(const unsigned int& i);

    void addFace(const unsigned int& i1, const unsigned int& i2, const unsigned int& i3);

    void computeBoundingBox();

    void computeBoundingSphere();

    const Math::Box3 &getBoundingBox();

    float height();

    float halfHeight();

    void allocVertices(const size_t& count);

    void allocIndices(const size_t& count);

    size_t getCountVertices();

    size_t getCountIndices();

    void freeVerties();

    void freeIndices();

    void setNeedUpdateVertices(bool);

    void setNeedUpdateIndices(bool);

    bool isNeedUpdateVertices();

    bool isNeedUpdateIndices();

    renderer::GeometryHandle *renderHandle = nullptr;

protected:

    Math::Box3 boundingBox;
    GeometryVertices vertices_;
    GeometryIndices indices_;
    GeometryType type = Geometry_Static;

private:


    bool needUpdateVertices = false;
    bool needUpdateIndices = false;
};

#endif
