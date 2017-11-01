#ifndef MODEL_H_
#define MODEL_H_

#include <boost/thread.hpp>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../lib/console.h"
#include "../resources/resources.h"
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
//	Model(const aiScene *);
	Model(const Model& model);
	~Model();

	ModelId getId();

	void initFromAi(const Resource::Assimp * assimpResource);
	void addMesh(Mesh * mesh);
	void setMeshScale(vec3 scale);
	void setMeshRotate(vec3 rotate, float angle);
	std::vector<Mesh*>* getMeshes();
private:
	ModelId id_;
	std::vector<Mesh*>* meshes_;

	void processNode(aiNode *, const Resource::Assimp * assimpResource);
	void processMesh(aiMesh *, const Resource::Assimp * assimpResource);

	void createId();
	void allocMeshes(unsigned int);
	void freeMeshes();
};

#endif
