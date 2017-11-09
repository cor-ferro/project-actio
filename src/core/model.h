#ifndef MODEL_H_
#define MODEL_H_

#include <boost/thread.hpp>
#include <unordered_map>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../lib/console.h"
#include "../lib/types.h"
#include "../resources/resources.h"
#include "../animation/animation.h"
#include "./mesh.h"

typedef int ModelId;

static int idCounter = 0;
static ModelId newId()
{
	// @todo: limited int size
	console::info("create id");
	return ++idCounter;
}

struct Model {
	Model();
	Model(Resource::File);
	Model(Mesh * mesh);
	Model(const Model& model);
	~Model();

	ModelId getId();

	void initFromAi(const Resource::Assimp * assimpResource);
	void addMesh(Mesh * mesh);
	
	void scale(vec3 scale);
	void rotate(vec3 rotate, float angle);

	std::vector<Mesh*>* getMeshes();

	std::string currentAnimation;
private:
	void processNode(aiNode *, const Resource::Assimp * assimpResource, int depth);
	void processMesh(aiMesh *, const Resource::Assimp * assimpResource);

	void createId();
	void allocMeshes(unsigned int);
	void freeMeshes();

		ModelId id_;
	std::vector<Mesh*>* meshes_ = nullptr;
	std::unordered_map<std::string, Animation> animations;
};

#endif
