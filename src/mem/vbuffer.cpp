#include "vbuffer.h"

VBuffer::VBuffer()
{
    size = 0;
    totalSize = 0;
}

void VBuffer::alloc(size_t newSize)
{
    if (size != 0) {
        console::warn("buffer already allocated");
        return;
    }

    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, newSize, NULL, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, newSize, NULL, GL_STATIC_DRAW);

    glBindVertexArray(0);

    size = 0;
    totalSize = newSize;
};

bool VBuffer::add(void * data, size_t dataSize)
{
    if (size + dataSize > totalSize) {
        console::err("Failed add data to vbuffer. Reached buffer limit.");
        return false;
    }

    VBufferMap newSizeMap;
    newSizeMap.setOffset(0);
    newSizeMap.setSize(dataSize);

    if (sizeMap.size() != 0) {
        VBufferMap& prevSizeMap = sizeMap.back();

        newSizeMap.setOffset(prevSizeMap.getOffset() + prevSizeMap.getSize());
    }

    // glBindVertexArray(vbo);
    // glBufferSubData(GL_ARRAY_BUFFER, newSizeMap.getOffset(), newSizeMap.getSize(), data);
    // glBindVertexArray(0);

    // sizeMap.push_back(newSizeMap);
    // size+= newSizeMap.getSize();

    return true;
}

size_t VBuffer::getSize()
{
    return size;
}

size_t VBuffer::getTotalSize()
{
    return totalSize;
}

void VBuffer::clear()
{

}

void VBuffer::free()
{

}

void VBuffer::destroy()
{
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    size = 0;
    totalSize = 0;
    sizeMap.clear();
}