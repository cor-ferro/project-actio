#include "model.h"

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

ModelNode::ModelNode() : name("") {}
ModelNode::~ModelNode() {}

ModelNode::ModelNode(aiNode * node) 
	: name(std::string(node->mName.C_Str()))
{
	meshes.reserve(node->mNumMeshes);
	children.reserve(node->mNumChildren);

	transformation = libAi::toNativeType(node->mTransformation);
}

void ModelNode::addMesh(Mesh * mesh)
{
	meshes.push_back(std::shared_ptr<Mesh>(mesh));
}

void ModelNode::addNode(ModelNode * node)
{
	children.push_back(std::shared_ptr<ModelNode>(node));
}

Model * Model::Create()
{
	void * place = modelsAllocator->Allocate(sizeof(Model), 8);
	Model * model = new(place) Model();
//	_models.push_back(model);

	return model;
}

Model * Model::Create(Config& modelConfig)
{
	void * place = modelsAllocator->Allocate(sizeof(Model), 8);
	Model * model = new(place) Model(modelConfig);
//	_models.push_back(model);

	return model;
}

Model * Model::Create(Mesh * mesh)
{
	void * place = modelsAllocator->Allocate(sizeof(Model), 8);
	Model * model = new(place) Model(mesh);
//	_models.push_back(model);

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
{
	allocMeshes(1);
}

Model::Model(const Model& other)
{
	console::info("copy model");
	freeMeshes();
	id_ = other.id_;
	name_ = other.name_;
	meshes_ = other.meshes_;
	nodes_ = other.nodes_;
	animations_ = other.animations_;
	rootNode_ = other.rootNode_;
}

Model::Model(Mesh * mesh) {
	allocMeshes(1);
	addMesh(mesh);
	mesh->setup();
	mesh->material.setupTextures();
}

Model::Model(Model::Config& config)
	: id_(newId())
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

//		ozz::animation::offline::AnimationBuilder animationBuilder;
//		skeleton = createSkeleton(assimpResource);
//		animations.reserve(scene->mNumAnimations);
//
//		for (int i = 0; i < scene->mNumAnimations; i++)
//		{
//			RawAnimation * rawAnimation = createAnimation(scene->mAnimations[i]);
//			rawAnimations.push_back(rawAnimation);
//			animations.insert({ std::string(rawAnimation->name.c_str()), animationBuilder(*rawAnimation) });
//		}
	} else {
		console::warn("no animations");
	}

	console::infop("materials: %i", scene->mNumMaterials);
	console::infop("meshes: %i", scene->mNumMeshes);

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
	console::info("free model");
}

void Model::destroy()
{
	freeMeshes();
    ozz::memory::Allocator* allocator = ozz::memory::default_allocator();
    currentAnimation = nullptr;

    allocator->Delete(skeleton);

    for (auto rawAnimation : rawAnimations)
    	allocator->Delete(rawAnimation);

    std::unordered_map<std::string, ozz::animation::Animation*>::iterator it;

    for (it = animations.begin(); it != animations.end(); it++)
    	allocator->Delete(it->second);

//    allocator->Deallocate(locals_);
//    allocator->Deallocate(models_);
//    allocator->Delete(cache_);
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

	console::infop("nodes.size(): %i", nodes.size());

	std::queue<aiNode*> queueNodes;
	for (auto node : nodes)
		queueNodes.push(node);

	std::queue<std::string> queueImages;
	for (auto path : texturePaths)
		queueImages.push(path);

	assert(queueNodes.front() == assimpResource->getRootNode());

	rootNode_ = new ModelNode(queueNodes.front());
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

			ModelNode * modelNode = new ModelNode(node);
			this->addNode(modelNode);

			for (unsigned int i = 0; i < node->mNumMeshes; i++) {
				unsigned int meshIndex = node->mMeshes[i];
				const aiMesh * mesh = assimpResource->getMesh(meshIndex);
				const aiMaterial * meshMaterial = assimpResource->getMeshMaterial(mesh);

				Mesh * modelMesh = Mesh::Create();
				modelMesh->material.initFromAi(meshMaterial, assimpResource, images_);
				modelMesh->geometry.initFromAi(mesh, assimpResource);

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
	console::info("end init meshes");

	for(auto mesh = meshes_.begin(); mesh != meshes_.end(); mesh++)
	{
		(*mesh)->setup();
		(*mesh)->material.setupTextures();
	}

}

void Model::addMesh(Mesh * mesh)
{
	mesh->setParentObject(&object);
	meshes_.push_back(mesh);
}

void Model::addNode(ModelNode * node)
{
	nodes_.insert({ node->name, std::shared_ptr<ModelNode>(node) });
}

void Model::removeMesh(Mesh * mesh)
{
	ModelMeshes::iterator it = std::find_if(meshes_.begin(), meshes_.end(), [&mesh](const ModelMesh& ptr) {
		return ptr == mesh;
	});

	if (it != meshes_.end()) {
		(*it)->setParentObject(nullptr);
		meshes_.erase(it);
	}
}

void Model::removeNode(ModelNode * node)
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

RawSkeleton * Model::createSkeleton(const Resource::Assimp * assimpResource)
{
	const aiNode * rootNode = assimpResource->getRootNode();

	ozz::memory::Allocator* allocator = ozz::memory::default_allocator();

	RawSkeleton * skeleton = new RawSkeleton();
	skeleton->roots.resize(1);

	RawSkeleton::Joint * root = &skeleton->roots[0];
	root->name = rootNode->mName.C_Str();
	root->transform.translation = Float3(0.0f, 1.0f, 1.0f);
	root->transform.rotation = Quaternion::identity();
	root->transform.scale = Float3::one();

	createJoints(rootNode, root);

	return skeleton;
}

void Model::createJoints(const aiNode * node, RawSkeleton::Joint * joint)
{
	joint->children.resize(node->mNumChildren);

	for (uint i = 0; i < node->mNumChildren; i++) {
		RawSkeleton::Joint * childJoint = &joint->children[i];
		childJoint->name = node->mChildren[i]->mName.C_Str();
		childJoint->transform.translation = Float3::zero();
		childJoint->transform.rotation = Quaternion::FromAxisAngle(Float4(1.0f, 0.0f, 0.0f, 0.0f));
		childJoint->transform.scale = Float3::one();

		createJoints(node->mChildren[i], childJoint);
	}
}

RawAnimation * Model::createAnimation(aiAnimation * assimpAnimation)
{
	RawAnimation * animation = new RawAnimation();

	animation->duration = (float)(assimpAnimation->mDuration * assimpAnimation->mTicksPerSecond);
	animation->tracks.reserve(assimpAnimation->mNumChannels);
	animation->name = assimpAnimation->mName.C_Str();

	for (uint i = 0; i < assimpAnimation->mNumChannels; i++) {
		RawAnimation::JointTrack& track = animation->tracks[i];
		aiNodeAnim * nodeAnim = assimpAnimation->mChannels[i];

		for (uint j = 0; j < nodeAnim->mNumPositionKeys; j++) {
			aiVectorKey& mPositionKey = nodeAnim->mPositionKeys[j];
			aiVector3D& mVec = mPositionKey.mValue;

			const RawAnimation::TranslationKey tkey = {(float)mPositionKey.mTime, Float3(mVec.x, mVec.y, mVec.z)};

			track.translations.push_back(tkey);
		}

		for (uint j = 0; j < nodeAnim->mNumRotationKeys; j++) {
			aiQuatKey& mRotationKey = nodeAnim->mRotationKeys[j];
			aiQuaternion& mQuat = mRotationKey.mValue;

			const RawAnimation::RotationKey rkey = {(float)mRotationKey.mTime, Quaternion(mQuat.x, mQuat.y, mQuat.z, mQuat.w)};

			track.rotations.push_back(rkey);
		}

		for (uint j = 0; j < nodeAnim->mNumScalingKeys; j++) {
	        aiVectorKey& mScalingKey = nodeAnim->mScalingKeys[i];
	        aiVector3D& mVec = mScalingKey.mValue;

	        const RawAnimation::ScaleKey skey = {(float)mScalingKey.mTime, Float3(mVec.x, mVec.y, mVec.z)};

	        track.scales.push_back(skey);
		}
	}

	return animation;
}

void Model::setCurrentAnimation(std::string name) {
	auto it = animations.find(name);

	if (it == animations.end()) {
		return;
	}

	currentAnimation = it->second;
}
