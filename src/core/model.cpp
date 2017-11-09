#include "model.h"

Model::Model()
{
	createId();
	allocMeshes(0);
}

Model::Model(const Model& other)
{
	console::info("copy model");
	freeMeshes();
	id_ = other.id_;
	meshes_ = other.meshes_;
}

Model::Model(Mesh * mesh)
{
	allocMeshes(1);
	// mesh->setScale(vec3(15.0));
	mesh->setup();
	mesh->material.setupTextures();
	addMesh(mesh);
}

Model::Model(Resource::File fileResource)
{
	Assimp::Importer importer;

	std::string pFile = fileResource.getPath();
	unsigned int flags = aiProcessPreset_TargetRealtime_Quality | aiProcess_Triangulate/* | aiProcess_FlipUVs*/;
	const aiScene * scene = importer.ReadFile(pFile, flags);
	const Resource::Assimp * assimpResource = new Resource::Assimp(scene, pFile);

	if(!scene)
	{
		console::err("failed model loading", importer.GetErrorString());
		return;
	}

	// processNode(scene->mRootNode, assimpResource, 1);

	if (scene->mNumAnimations > 0) {
		console::info("animations:", scene->mNumAnimations);
		for (int i = 0; i < scene->mNumAnimations; i++)
		{
			const aiAnimation * animation = scene->mAnimations[i];
			console::info("+ animation:", animation->mName.C_Str());

			if (animation->mNumChannels > 0)
			{
				console::info("++ num channels:", animation->mNumChannels);
				for (int j = 0; j < animation->mNumChannels; j++)
				{
					const aiNodeAnim * channel = animation->mChannels[i];
					console::info("++ channel", channel->mNodeName.C_Str());
					console::info("++ keys", channel->mNumPositionKeys, channel->mNumRotationKeys, channel->mNumScalingKeys);
					// console::info("++ mNumPositionKeys", channel->mNumPositionKeys);
					// for (int k = 0; k < channel->mNumPositionKeys; k++) {
					// 	console::info("++ mPositionKeys mTime", (float)channel->mPositionKeys[k].mTime);
					// }
					// for (int k = 0; k < channel->mNumRotationKeys; k++) {
					// 	console::info("++ mRotationKeys mTime", (float)channel->mRotationKeys[k].mTime);
					// }
					// for (int k = 0; k < channel->mNumScalingKeys; k++) {
					// 	console::info("++ mScalingKeys mTime", (float)channel->mScalingKeys[k].mTime);
					// }
					console::info(" ");
				}
			} else {
				console::warn("no channels");
			}
		}

		currentAnimation = scene->mAnimations[scene->mNumAnimations - 1]->mName.C_Str();
		console::info("currentAnimation", currentAnimation);
	} else {
		console::warn("no animations");
	}

	console::info("materials", scene->mNumMaterials);
	console::info("meshes", scene->mNumMeshes);

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
		processMesh(assimpResource->scene->mMeshes[i], assimpResource);
		
		// tg.add_thread(
		// 	new boost::thread(
		// 		&Model::processMesh, this, assimpResource->scene->mMeshes[i], assimpResource
		// 	)
		// );
	}

	// tg.join_all();

	for(auto mesh = meshes_->begin(); mesh != meshes_->end(); mesh++)
	{
		(*mesh)->setup();
		(*mesh)->material.setupTextures();
	}
}

void Model::addMesh(Mesh * mesh)
{
	meshes_->push_back(mesh);
}

void Model::processNode(aiNode * node, const Resource::Assimp * assimpResource, int depth = 1)
{
	std::string indent = std::string(depth, '+');
	console::info(indent, "node", node->mName.C_Str());
	console::info(indent, "mNumMeshes", node->mNumMeshes);

	for (int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], assimpResource, ++depth);
	}
}


void Model::processMesh(aiMesh * mesh, const Resource::Assimp * assimpResource)
{
	Mesh * modelMesh = new Mesh(mesh, assimpResource);
	// modelMesh->setup();
	addMesh(modelMesh);
}

std::vector<Mesh*>* Model::getMeshes()
{
	return meshes_;
}

void Model::scale(vec3 scale) {
	for(auto mesh = meshes_->begin(); mesh != meshes_->end(); mesh++)
	{
		(*mesh)->setScale(scale);
	}
}

void Model::rotate(vec3 rotate, float angle) {
	for(auto mesh = meshes_->begin(); mesh != meshes_->end(); mesh++)
	{
		(*mesh)->rotate(rotate, angle);
	}
}
