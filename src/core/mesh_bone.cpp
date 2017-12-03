#include "mesh_bone.h"

MeshBone::MeshBone()
	: offset(mat4(1.0))
	, transform(mat4(1.0))
	, index(0)
{

}

MeshBone::MeshBone(aiBone * bone) 
	: offset(libAi::toNativeType(bone->mOffsetMatrix))
	, transform(mat4(1.0))
	, index(0)
{

}

unsigned int MeshBone::getIndex() const
{
	return index;
}

mat4 MeshBone::getTransform() const
{
	return transform;
}

mat4 MeshBone::getOffset() const
{
	return offset;
}

void MeshBone::setOffset(mat4& newOffset)
{
	offset = newOffset;
}

void MeshBone::setTransform(mat4& newTransform)
{
	transform = newTransform;
}

void MeshBone::setIndex(unsigned int& newIndex)
{
	index = newIndex;
}