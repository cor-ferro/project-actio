#include "handle.h"

renderer::Opengl::GeometryHandle::GeometryHandle(::Geometry *geometry) : geometry(geometry) {
    setup();
}

renderer::Opengl::GeometryHandle::~GeometryHandle() {
    destroy();
}

void renderer::Opengl::GeometryHandle::setup() {
    if (ready) {
        console::warn("geometry already setups");
        return;
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    GeometryVertices *vertices = geometry->getVertices();

    switch (geometry->getType()) {
        case Geometry::Geometry_Static:
            draw = GL_STATIC_DRAW;
            break;
        case Geometry::Geometry_Dynamic:
            draw = GL_DYNAMIC_DRAW;
            break;
        default:
            console::warn("Unknown draw type");
            draw = GL_STATIC_DRAW;
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(Vertex), &vertices->front(), draw);

    GeometryIndices *indices = geometry->getIndices();

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size() * sizeof(MeshIndex), &indices->front(), draw);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, TexCoords));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Tangent));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Bitangent));

    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void *) offsetof(Vertex, BonedIDs));

    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Weights));

    glBindVertexArray(0);

    geometry->renderHandle = this;
    ready = true;
}

void renderer::Opengl::GeometryHandle::destroy() {
    if (vbo != 0) glDeleteBuffers(1, &vbo);
    if (ebo != 0) glDeleteBuffers(1, &ebo);
    if (vao != 0) glDeleteVertexArrays(1, &vao);

    ready = false;
    geometry->renderHandle = nullptr;
}

void renderer::Opengl::GeometryHandle::reset(::Geometry *geometry) {
    if (ready) {
        destroy();
    }

    this->geometry = geometry;
    setup();
}
