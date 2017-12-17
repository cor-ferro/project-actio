#ifndef VMEM_H_
#define VMEM_H_

#include "../lib/console.h"
#include "vbuffer.h"

struct VMem {
    VMem() {
        
    };

    ~VMem() {
        console::warn("free video memory buffers");
        bufferGeometry.destroy();
        miscGeometry.destroy();
    }

    void allocGeometry() {
        bufferGeometry.alloc(1024 * 1024 * 5); // 5mb
        miscGeometry.alloc(1024 * 1024 * 100); // 50mb
    }

    VBuffer bufferGeometry;
    VBuffer miscGeometry;
};

#endif