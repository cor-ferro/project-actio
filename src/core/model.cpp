#include <algorithm>
#include <thread>
#include <mutex>
#include <queue>
#include <unordered_set>
#include "model.h"
#include "../lib/console.h"
#include "../lib/ozz.h"
#include "../lib/assimp.h"
#include "../math/Box3.h"

// #define THREAD_INIT_AI_

Model::Config::Config()
	: name("")
	, flipUv(false)
	, position(vec3(0.0f))
	, rotation(vec3(1.0f))
	, rotationAngle(0.0f)
	, scale(vec3(1.0f))
{}

Model::Config::Config(const Model::Config& other)
{
	name = other.name;
	file = other.file;
	flipUv = other.flipUv;
	position = other.position;
	rotation = other.rotation;
	rotationAngle = other.rotationAngle;
	scale = other.scale;
	animation = other.animation;
}

Model::Node::Node() : name("") {}
Model::Node::~Node() {}

Model::Node::Node(aiNode * node)
	: name(std::string(node->mName.C_Str()))
{
	meshes.reserve(node->mNumMeshes);
	children.reserve(node->mNumChildren);

	transformation = libAi::toNativeType(node->mTransformation);
}

void Model::Node::addMesh(Mesh * mesh)
{
	meshes.push_back(mesh);
}

void Model::Node::addNode(Model::Node * node)
{
	children.push_back(std::shared_ptr<Model::Node>(node));
}

Model * Model::Create()
{
	void * place = modelsAllocator->Allocate(sizeof(Model), 8);
	Model * model = new(place) Model();

	return model;
}

Model * Model::Create(Config& modelConfig)
{
	void * place = modelsAllocator->Allocate(sizeof(Model), 8);
	Model * model = new(place) Model(modelConfig);

	return model;
}

Model * Model::Create(Mesh * mesh)
{
	void * place = modelsAllocator->Allocate(sizeof(Model), 8);
	Model * model = new(place) Model(mesh);

	return model;
}

void Model::Destroy(Model * model)
{
	if (model == nullptr) return;

	model->destroy();
	model->~Model();
	modelsAllocator->Free((void*)model);
}

Model::Model()
	: id_(newId())
	, name_()
	, skeleton(nullptr)
	, currentAnimation(nullptr)
{
    console::info("alloc new model");
}

Model::Model(const Model& other)
{
	console::info("copy model by reference");

	id_ = other.id_;
	name_ = other.name_;
	meshes_ = other.meshes_;
	nodes_ = other.nodes_;
	animations_ = other.animations_;
	rootNode_ = other.rootNode_;
}

Model::Model(const Model* other)
{
    console::info("copy model by pointer");

    console::info("%i : %i", id_, other->id_);
    console::info("%i, %i, %i, %i", id_, other->meshes_.size(), other->nodes_.size(), other->animations_.size());
    assert(id_ == other->id_);

    id_ = other->id_;
    name_ = other->name_;
    meshes_ = other->meshes_;
    nodes_ = other->nodes_;
    animations_ = other->animations_;
    rootNode_ = other->rootNode_;
}

Model::Model(Mesh * mesh) : Model() {
	allocMeshes(1);
	addMesh(mesh);
	mesh->setup();
	mesh->material.setupTextures();
}

Model::Model(Model::Config& config) : Model()
{
	Assimp::Importer importer;

	std::string pFile = config.file.getPath();
	unsigned int flags = aiProcessPreset_TargetRealtime_Quality | aiProcess_GenSmoothNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace;

	if (config.flipUv) {
		flags|= aiProcess_FlipUVs;
	}

	const aiScene * scene = importer.ReadFile(pFile, flags);
	const Resource::Assimp * assimpResource = new Resource::Assimp(scene, pFile);

	if(!scene)
	{
		console::err("failed model loading", importer.GetErrorString());
		return;
	}

	if (scene->mNumAnimations > 0) {
		console::info("animations: ", scene->mNumAnimations);

		ozz::animation::offline::SkeletonBuilder skeletonBuilder;

		RawSkeleton rawSkeleton;
		createSkeleton(assimpResource, &rawSkeleton);

		skeleton = skeletonBuilder(rawSkeleton);

		for (int i = 0; i < scene->mNumAnimations; i++)
		{
			animation::Animation* animation = new animation::Animation(scene->mAnimations[i], skeleton);

			if (animation->valid()) {
				animations_.insert({ animation->getName(), animation });
			}
		}
	} else {
		console::warn("no animations");
	}

	console::info("materials: %i", scene->mNumMaterials);
	console::info("meshes: %i", scene->mNumMeshes);

	allocMeshes(0);
	initFromAi(assimpResource);

	setName(config.name);
	setPosition(config.position);
	rotate(config.rotation, config.rotationAngle);
	setScale(config.scale);

	if (config.animation != "") {
		setCurrentAnimation(config.animation);
	}

	importer.FreeScene();
	delete assimpResource;
}

Model::~Model()
{
//	console::info("free model %i", id_);
}

void Model::destroy()
{
	freeMeshes();

	currentAnimation = nullptr;

	if (skeleton != nullptr) {
		console::info("destroy skeleton");
		ozz::memory::Allocator* allocator = ozz::memory::default_allocator();

		for (auto &pair : animations_) {
			delete pair.second;
			pair.second = nullptr;
		}

		animations_.erase(animations_.begin(), animations_.end());
		currentAnimation = nullptr;

		allocator->Delete(skeleton);
		skeleton = nullptr;
	}
}

const ModelId& Model::getId()
{
	return id_;
}

const ModelName& Model::getName()
{
	return name_;
}

void Model::setName(std::string newName)
{
	name_ = newName;
}

void Model::allocMeshes(unsigned int count)
{
	meshes_.clear();
	meshes_.reserve(count);
}

void Model::freeMeshes()
{
	for (auto mesh : meshes_) {
		Mesh::Destroy(mesh);
	}

	meshes_.clear();
}

void Model::freeNodes()
{
	// @todo
}

void Model::initFromAi(const Resource::Assimp * assimpResource)
{
	freeMeshes();
	allocMeshes(assimpResource->scene->mNumMeshes);

	std::unordered_set<std::string> texturePaths = assimpResource->getTexturePaths();
	std::vector<aiNode*> nodes = assimpResource->getAllNodes();

	console::info("nodes.size(): %i", nodes.size());

	std::queue<aiNode*> queueNodes;
	for (auto node : nodes)
		queueNodes.push(node);

	std::queue<std::string> queueImages;
	for (auto path : texturePaths)
		queueImages.push(path);

	assert(queueNodes.front() == assimpResource->getRootNode());

	rootNode_ = new Model::Node(queueNodes.front());
	queueNodes.pop();

	std::mutex lock;
	auto nodeLoader = [this, &lock, &assimpResource](std::queue<aiNode*>& queue)
	{
		aiNode * node = nullptr;
		while (queue.size() > 0) {
			lock.lock();
			node = queue.front();
			queue.pop();
			lock.unlock();
//			console::info(std::this_thread::get_id());

            Model::Node * modelNode = new Model::Node(node);
			this->addNode(modelNode); // todo: atomic insert

			for (unsigned int i = 0; i < node->mNumMeshes; i++) {
				unsigned int meshIndex = node->mMeshes[i];
				const aiMesh * mesh = assimpResource->getMesh(meshIndex);
				const aiMaterial * meshMaterial = assimpResource->getMeshMaterial(mesh);

				Mesh * modelMesh = Mesh::Create();
				modelMesh->material.initFromAi(meshMaterial, assimpResource, images_);
				modelMesh->geometry.initFromAi(mesh, assimpResource);

				modelMesh->bones.resize(mesh->mNumBones);

				if (mesh->mNumBones > 0) {
					std::vector<BoneMap> boneJointsMap;
					ozz::Range<const char* const> jointNames = skeleton->joint_names();

					for (uint boneId = 0; boneId < mesh->mNumBones; boneId++) {
						aiBone * bone = mesh->mBones[boneId];
						const char* boneName = bone->mName.C_Str();
						uint jointIndex = 0;

						for (uint jIndex = 0; jIndex < jointNames.Count(); jIndex++) {
							if (strcmp(jointNames[jIndex], boneName) == 0) {
								jointIndex = jIndex;
								break;
							}
						}

						BoneMap map;
						map.jointIndex = jointIndex;
						map.boneIndex = boneId;
						map.offset = libAi::toNativeType(bone->mOffsetMatrix);

						boneJointsMap.push_back(map);
					}

					boneMeshMap.insert({ modelMesh, boneJointsMap });
				}

				this->addMesh(modelMesh);
				modelNode->addMesh(modelMesh);
			}

			// todo: выстраивать иерархию нод
		}
	};

	auto imageLoader = [this, &lock, &assimpResource](std::queue<std::string>& queue)
	{
		std::string path("");
		while (queue.size() > 0) {
			lock.lock();
			path = queue.front();
			queue.pop();
			lock.unlock();

			ImageLoader::Data imageData = ImageLoader::load(path);

			if (!imageData.isReady()) {
				imageData = ImageLoader::load(assimpResource->getDefaultTexturePath());
			}

			this->images_.insert({ path, imageData });
		}
	};

	const uint countThreads = 1;
	std::vector<std::thread> nodeThreads;
	std::vector<std::thread> imageThreads;

	console::info("start load images");
	for (uint i = 0; i < countThreads; i++)
		imageThreads.push_back(std::thread(imageLoader, std::ref(queueImages)));
	for (uint i = 0; i < countThreads; i++)
		imageThreads[i].join();
	console::info("end load images");

	console::info("start init meshes");
	for (uint i = 0; i < countThreads; i++)
		nodeThreads.push_back(std::thread(nodeLoader, std::ref(queueNodes)));
	for (uint i = 0; i < countThreads; i++)
		nodeThreads[i].join();
	console::info("end init meshes %i", nodes_.size());

	for(auto mesh = meshes_.begin(); mesh != meshes_.end(); mesh++)
	{
		(*mesh)->setup();
		(*mesh)->material.setupTextures();
	}

}

void Model::addMesh(Mesh * mesh)
{
	meshes_.push_back(mesh);
}

void Model::addNode(Node * node)
{
	nodes_.insert({ node->name, std::shared_ptr<Node>(node) });
}

void Model::removeMesh(Mesh * mesh)
{
	ModelMeshes::iterator it = std::find_if(meshes_.begin(), meshes_.end(), [&mesh](const Mesh* ptr) {
		return ptr == mesh;
	});

	if (it != meshes_.end()) {
		meshes_.erase(it);
	}
}

void Model::removeNode(Node * node)
{
	auto it = nodes_.find(node->name);

	if (it != nodes_.end()) {
		nodes_.erase(it);
	}
}

const ModelMeshes& Model::getMeshes()
{
	return meshes_;
}

Mesh * Model::getFirstMesh()
{
	return meshes_.at(0);
}

const int Model::getNodesCount()
{
	return nodes_.size();
}

void Model::setScale(vec3 scale)
{
	object.setScale(scale);
}

void Model::rotate(vec3 rotate, float angle)
{
	object.rotate(rotate, angle);
}

void Model::setPosition(vec3 position)
{
	object.setPosition(position);
}

void Model::setPosition(float x, float y, float z)
{
	object.setPosition(x, y, z);
}

void Model::setQuaternion(float x, float y, float z, float w)
{
	object.setQuaternion(x, y, z, w);
}

void Model::createSkeleton(const Resource::Assimp * assimpResource, RawSkeleton * rawSkeleton)
{
	const aiNode * rootNode = assimpResource->getRootNode();

	ozz::memory::Allocator* allocator = ozz::memory::default_allocator();

	rawSkeleton->roots.resize(1);

	RawSkeleton::Joint * root = &rawSkeleton->roots[0];
	root->name = rootNode->mName.C_Str();

	aiMatrix4x4 mat = rootNode->mTransformation;
	aiVector3D translation, scale;
	aiQuaternion rotation;
	mat.Decompose(scale, rotation, translation);

	// root->transform.translation = Float3((float)translation.x, (float)translation.y, (float)translation.z);
	// root->transform.rotation = Quaternion((float)rotation.x, (float)rotation.y, (float)rotation.z, (float)rotation.w);
	// root->transform.scale = Float3((float)scale.x, (float)scale.y, (float)scale.z);

	createJoints(rootNode, root);
}

void Model::createJoints(const aiNode * node, RawSkeleton::Joint * joint)
{
	joint->children.resize(node->mNumChildren);

	for (uint i = 0; i < node->mNumChildren; i++) {
		RawSkeleton::Joint * childJoint = &joint->children[i];
		childJoint->name = node->mChildren[i]->mName.C_Str();
		
		aiMatrix4x4 mat = node->mTransformation;
		aiVector3D translation, scale;
		aiQuaternion rotation;
		mat.Decompose(scale, rotation, translation);
		
		// childJoint->transform.translation = Float3((float)translation.x, (float)translation.y, (float)translation.z);
		// childJoint->transform.rotation = Quaternion((float)rotation.x, (float)rotation.y, (float)rotation.z, (float)rotation.w);
		// childJoint->transform.scale = Float3((float)scale.x, (float)scale.y, (float)scale.z);

		createJoints(node->mChildren[i], childJoint);
	}
}

void Model::setCurrentAnimation(std::string name) {
	auto it = animations_.find(name);

	if (it == animations_.end()) {
		console::warn("animation not found %s", name.c_str());
		return;
	}

	currentAnimation = it->second;
}

void Model::tickAnimationTime(float time)
{
	if (currentAnimation == nullptr) {
		return;
	}

	currentAnimation->tick(time);
}

void Model::processAnimation()
{
	if (currentAnimation == nullptr) {
		return;
	}

	animation::process(currentAnimation, skeleton);

	mat4* bones_ = currentAnimation->getBones();

	for (auto const& x : boneMeshMap) {
		Mesh * mesh = x.first;

		for (const BoneMap& jointBoneMap : x.second) {
			mesh->bones.transforms[jointBoneMap.boneIndex] = bones_[jointBoneMap.jointIndex] * jointBoneMap.offset;
		}
	}
}

bool Model::isAnimationProgress()
{
	return currentAnimation != nullptr;
}