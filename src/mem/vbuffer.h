#ifndef VBUFFER_H_
#define VBUFFER_H_

#include <glad/glad.h>
#include "stddef.h"
#include <vector>
#include "../lib/console.h"
#include "vbuffermap.h"

struct VBuffer {
    VBuffer();

    void alloc(size_t newSize);
    bool add(void* data, size_t dataSize);
    void clear();
    void free();
    void destroy();

    size_t getSize();
    size_t getTotalSize();

    private:
        GLuint vbo;
        GLuint ebo;
        size_t size;
        size_t totalSize;
        std::vector<VBufferMap> sizeMap;
};

#endif