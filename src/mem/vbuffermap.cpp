#include "vbuffermap.h"

VBufferMap::VBufferMap()
{
    offset = 0;
    size = 0;
}

void VBufferMap::fromMap(VBufferMap& other)
{
    setOffset(other.getOffset() + other.getSize());
}

void VBufferMap::first()
{
    offset = 0;
}

size_t VBufferMap::getOffset()
{
    return offset;
}

size_t VBufferMap::getSize()
{
    return size;
}

void VBufferMap::setOffset(size_t newOffset)
{
    offset = newOffset;
}

void VBufferMap::setSize(size_t newSize)
{
    size = newSize;
}