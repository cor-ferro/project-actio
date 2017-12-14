#ifndef CORE_MESH_BONE_H_
#define CORE_MESH_BONE_H_

#include <glad/glad.h>

#include <map>
#include <vector>
#include <assimp/scene.h>
#include "../lib/types.h"
#include "../lib/assimp.h"

typedef std::pair<uint, float> BoneVertexWeight;
typedef std::map<uint, std::vector<BoneVertexWeight>> VerticesMap;

struct MeshBone {
	MeshBone();
	MeshBone(aiBone * bone);

	unsigned int getIndex() const;
	mat4 getTransform() const;
	mat4 getOffset() const;

	void setOffset(mat4& newOffset);
	void setTransform(mat4& newTransform);
	void setIndex(unsigned int& newIndex);

private:
	unsigned int index;
	mat4 offset;
	mat4 transform;
};

#endif