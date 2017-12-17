#ifndef VBUFFERMAP_H_
#define VBUFFERMAP_H_

#include "stddef.h"

struct VBufferMap {
    VBufferMap();
    void first();

    void fromMap(VBufferMap& other);

    size_t getOffset();
    size_t getSize();

    void setOffset(size_t offset);
    void setSize(size_t size);

    private:
        size_t offset;
        size_t size;
};

#endif