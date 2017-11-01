#include "model.h"

void modelThread()
{
	console::info("model thread");
}

Model::Model()
{
	createId();
	allocMeshes(0);
}

//Model::Model(const aiScene * scene)
//{
//	createId();
//	allocMeshes(0);
//	initFromAi(scene);
//}

Model::Model(const Model& other)
{
	console::info("copy model");
	freeMeshes();
	id_ = other.id_;
	meshes_ = other.meshes_;
}

Model::Model(Resource::File fileResource)
{
	Assimp::Importer importer;

	std::string pFile = fileResource.getPath();
	unsigned int flags = aiProcessPreset_TargetRealtime_Fast | aiProcess_CalcTangentSpace;
	const aiScene * scene = importer.ReadFile(pFile, flags);
	const Resource::Assimp * assimpResource = new Resource::Assimp(scene, pFile);

	if(!scene)
	{
		console::err("failed model loading", importer.GetErrorString());
		return;
	}

	if (scene->mNumAnimations > 0) {
		console::info("animations", scene->mNumAnimations);
		for (int i = 0; i < scene->mNumAnimations; i++)
		{
			const aiAnimation * animation = scene->mAnimations[i];
			console::info(" + ", animation->mName.C_Str(), "duration:", animation->mDuration, "Ticks per second", animation->mTicksPerSecond);
		}
	}

	console::info("materials", scene->mNumMaterials);
	console::info("meshes", scene->mNumMeshes);
	console::info("textures", scene->mNumTextures);

	createId();
	console::info("alloc meshes");
	allocMeshes(0);
	console::info("init from ai");
	initFromAi(assimpResource);
	console::info("init from ai done");

	importer.FreeScene();
	delete assimpResource;
}

Model::~Model()
{
	console::info("free model");
	freeMeshes();
}

ModelId Model::getId()
{
	return id_;
}

void Model::createId()
{
	id_ = newId();
}

void Model::allocMeshes(unsigned int count)
{
	meshes_ = new std::vector<Mesh*>();
	meshes_->reserve(count);
}

void Model::freeMeshes()
{
	if (meshes_ != 0) {
		delete meshes_;
		meshes_ = 0;
	}
}

void Model::initFromAi(const Resource::Assimp * assimpResource)
{
	freeMeshes();
	allocMeshes(assimpResource->scene->mNumMeshes);

	boost::thread_group tg;

	unsigned int numMeshes = assimpResource->scene->mNumMeshes;
	for (unsigned int i = 0; i < numMeshes; i++) {
		// processMesh(assimpResource->scene->mMeshes[i], assimpResource);
		
		tg.add_thread(
			new boost::thread(
				&Model::processMesh, this, assimpResource->scene->mMeshes[i], assimpResource
			)
		);
	}

	tg.join_all();

	for(auto mesh = meshes_->begin(); mesh != meshes_->end(); mesh++)
	{
		(*mesh)->setup();
		(*mesh)->material.texturesBindData();
	}

	// processNode(assimpResource->scene->mRootNode, assimpResource);
}

void Model::addMesh(Mesh * mesh)
{
	meshes_->push_back(mesh);
}

void Model::processNode(aiNode * node, const Resource::Assimp * assimpResource)
{
	// unsigned int numMeshes = node->mNumMeshes;
	// for (unsigned int i = 0; i < numMeshes; i++) {
	// 	aiMesh * mesh = assimpResource->scene->mMeshes[node->mMeshes[i]];
	// 	processMesh(mesh, assimpResource);
	// }

	// unsigned int numChildren = node->mNumChildren;
	// for (unsigned int i = 0; i < numChildren; i++) {
	// 	processNode(node->mChildren[i], assimpResource);
	// }
}


void Model::processMesh(aiMesh * mesh, const Resource::Assimp * assimpResource)
{
	Mesh * modelMesh = new Mesh(mesh, assimpResource);
	// modelMesh->setup();
	addMesh(modelMesh);
	// console::info("add mesh");
}

std::vector<Mesh*>* Model::getMeshes()
{
	return meshes_;
}

void Model::setMeshScale(vec3 scale) {
	for(auto mesh = meshes_->begin(); mesh != meshes_->end(); mesh++)
	{
		(*mesh)->setScale(scale);
	}
}

void Model::setMeshRotate(vec3 rotate, float angle) {
	for(auto mesh = meshes_->begin(); mesh != meshes_->end(); mesh++)
	{
		(*mesh)->rotate(rotate, angle);
	}
}
