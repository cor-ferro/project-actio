#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <glad/glad.h>

#include <vector>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtx/normal.hpp>
#include <glm/gtc/constants.hpp>
#include "../lib/types.h"
#include "../lib/console.h"
#include "../lib/mem.h"
#include "../lib/assimp.h"
#include "../resources/resources.h"
#include "../math/Box3.h"
#include "../math/Sphere.h"
#include "vertex.h"

#ifdef GRAPHIC_API_OPENGL

#include "../renderer/opengl/program.h"
#include "../renderer/opengl/uniforms.h"

#endif

typedef unsigned int MeshIndex;
typedef std::vector<Vertex> GeometryVertices;
typedef std::vector<MeshIndex> GeometryIndices;

static size_t TotalCountVertices = 0;

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

    Geometry(aiMesh *mesh, const Resource::Assimp *assimpResource);

    ~Geometry();

    void destroy();

    static int VertexSize() { return sizeof(Vertex); }

    static void AddTotalCountVertices(size_t count) { TotalCountVertices += count; }

    static void SubTotalCountVertices(size_t count) { TotalCountVertices -= count; }

    static size_t GetTotalCountVertices() { return TotalCountVertices; }

    void initFromAi(const aiMesh *mesh, const Resource::Assimp *assimpResource);

    GeometryVertices *getVertices();

    GeometryIndices *getIndices();

    void setType(GeometryType newType);

    GeometryType getType();

    void setVertices(std::vector<vec3> vertices);

    void addVertex(Vertex vertex);

    void addVertex(float x, float y, float z);

    void addVertex(std::vector<float> &vertices);

    Vertex &getVertex(unsigned int index);

    void fillIndices();

    void addIndex(unsigned int i);

    void addFace(unsigned int i1, unsigned int i2, unsigned int i3);

    void setup();

    void setupVertex(Vertex &v);

    void computeBoundingBox();

    void computeBoundingSphere();

    const Math::Box3 &getBoundingBox();

    float height();

    float halfHeight();

    void allocVertices(unsigned int count);

    void allocIndices(unsigned int count);

    size_t getCountVertices();

    size_t getCountIndices();

    void freeVerties();

    void freeIndices();

    void setNeedUpdateVertices(bool);

    void setNeedUpdateIndices(bool);

    bool isNeedUpdateVertices();

    bool isNeedUpdateIndices();

    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;

protected:

    Math::Box3 boundingBox;
    std::shared_ptr<GeometryVertices> vertices_;
    std::shared_ptr<GeometryIndices> indices_;
    GeometryType type = Geometry_Static;

private:
    Geometry(const Geometry &geometry);

    bool needUpdateVertices = false;
    bool needUpdateIndices = false;
};

#endif
