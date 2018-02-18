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
	meshes.push_back(mesh);
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
	, skeleton(nullptr)
	, currentAnimation(nullptr)
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
	bones_ = other.bones_;
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
			ozz::animation::offline::AnimationBuilder animationBuilder;

			RawAnimation rawAnimation;
			createAnimation(scene->mAnimations[i], &rawAnimation);

			ozz::animation::Animation * animation = animationBuilder(rawAnimation);

			if (!animation) {
				console::warnp("failed create animation %s, %i", scene->mAnimations[i]->mName.C_Str(), rawAnimation.Validate());
				break;
			}

			console::infop("animation: %s", scene->mAnimations[i]->mName.C_Str());
			animations.insert({ std::string(scene->mAnimations[i]->mName.C_Str()), animation });

			break;
		}

		ozz::memory::Allocator* allocator = ozz::memory::default_allocator();

		const int numSoaJoints = skeleton->num_soa_joints();
		const int numJoints = rawSkeleton.num_joints();

		console::infop("num_joints: %i", numJoints);

		locals_ = allocator->AllocateRange<ozz::math::SoaTransform>(numSoaJoints);
		bones_ = allocator->AllocateRange<ozz::math::Float4x4>(numJoints);
		cache_ = allocator->New<ozz::animation::SamplingCache>(numJoints);
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
//	console::infop("free model %i", id_);
}

void Model::destroy()
{
	freeMeshes();

	currentAnimation = nullptr;

	if (skeleton != nullptr) {
		console::info("destroy skeleton");
		ozz::memory::Allocator* allocator = ozz::memory::default_allocator();

		for (auto rawAnimation : rawAnimations)
			allocator->Delete(rawAnimation);

		std::unordered_map<std::string, ozz::animation::Animation*>::iterator it;
		for (it = animations.begin(); it != animations.end(); it++)
			allocator->Delete(it->second);

		allocator->Delete(skeleton);
		skeleton = nullptr;

		allocator->Deallocate(locals_);
		allocator->Deallocate(bones_);
		allocator->Delete(cache_);
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
			this->addNode(modelNode); // todo: atomic insert

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
	console::info("end init meshes", nodes_.size());

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
	root->transform.translation = Float3(0.0f, 1.0f, 1.0f);
	root->transform.rotation = Quaternion::identity();
	root->transform.scale = Float3::one();

	createJoints(rootNode, root);
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

void Model::createAnimation(aiAnimation * assimpAnimation, ozz::animation::offline::RawAnimation * rawAnimation)
{
	rawAnimation->duration = (float)(assimpAnimation->mDuration / assimpAnimation->mTicksPerSecond);
	rawAnimation->tracks.resize(assimpAnimation->mNumChannels);
//	rawAnimation->name = assimpAnimation->mName.C_Str();

	console::infop("duration %f", rawAnimation->duration);
	console::infop("mDuration %f", assimpAnimation->mDuration);
	console::infop("mTicksPerSecond %f", assimpAnimation->mTicksPerSecond);

	for (uint i = 0; i < assimpAnimation->mNumChannels; i++) {
		RawAnimation::JointTrack& track = rawAnimation->tracks[i];
		aiNodeAnim * nodeAnim = assimpAnimation->mChannels[i];

		track.translations.resize(nodeAnim->mNumPositionKeys);
		track.rotations.resize(nodeAnim->mNumRotationKeys);
		track.scales.resize(nodeAnim->mNumScalingKeys);

		float previousTranslationTime = -1.0f;
		for (uint j = 0; j < nodeAnim->mNumPositionKeys; j++) {
			aiVectorKey& mPositionKey = nodeAnim->mPositionKeys[j];
			aiVector3D& mVec = mPositionKey.mValue;

			float time = (float)(mPositionKey.mTime / assimpAnimation->mTicksPerSecond);
			assert(time >= 0.0f && time <= rawAnimation->duration && "translation");

			if (time <= previousTranslationTime) {
				continue;
			}
			const RawAnimation::TranslationKey tkey = {time, Float3((float)mVec.x, (float)mVec.y, (float)mVec.z)};

			track.translations[j] = tkey;

			previousTranslationTime = time;
		}

		for (uint j = 0; j < nodeAnim->mNumRotationKeys; j++) {
			aiQuatKey& mRotationKey = nodeAnim->mRotationKeys[j];
			aiQuaternion& mQuat = mRotationKey.mValue;

			float time = (float)(mRotationKey.mTime  / assimpAnimation->mTicksPerSecond);
			assert(time >= 0.0f && time <= rawAnimation->duration && "rotate");
			quat q = libAi::toNativeType(mQuat);
//			q = glm::normalize(q);
			float length = glm::length(q);

			const RawAnimation::RotationKey rkey = {time, ozz::math::Quaternion::identity()};
			track.rotations[j] = rkey;

//			track.rotations[j].time = time;
//			track.rotations[j].value = ozz::math::Quaternion::identity();
//			track.rotations[j].value.w = (float)mQuat.w;
//			track.rotations[j].value.x = (float)mQuat.x;
//			track.rotations[j].value.y = (float)mQuat.y;
//			track.rotations[j].value.z = (float)mQuat.z;

//			console::infop("rotation: %f %f %f %f, %f", mQuat.w, mQuat.x, mQuat.y, mQuat.z, length);
		}

		for (uint j = 0; j < nodeAnim->mNumScalingKeys; j++) {
	        aiVectorKey& mScalingKey = nodeAnim->mScalingKeys[j];
	        aiVector3D& mVec = mScalingKey.mValue;

	        float time = (float)(mScalingKey.mTime / assimpAnimation->mTicksPerSecond);
	        assert(time >= 0.0f && time <= rawAnimation->duration);
	        const RawAnimation::ScaleKey skey = {time, Float3((float)mVec.x, (float)mVec.y, (float)mVec.z)};

	        track.scales[j] = skey;
		}
	}

	console::infop("animation time: %f", rawAnimation->duration);
	console::infop("animation tracks: %i", rawAnimation->tracks.size());
	console::infop("Skeleton::kMaxJoints: %i", ozz::animation::Skeleton::kMaxJoints);

//	  if (rawAnimation->duration <= 0.f) {  // Tests duration is valid.
//		  console::warn("failed check track");
//	  }
//	  if (rawAnimation->tracks.size() > ozz::animation::Skeleton::kMaxJoints) {  // Tests number of tracks.
//		  console::warn("failed check track");
//	  }

//	  for (size_t j = 0; j < rawAnimation->tracks.size(); ++j) {
//		const RawAnimation::JointTrack& track = rawAnimation->tracks[j];
//
//		{
//			float previous_time = -1.f;
//
//			for (size_t k = 0; k < track.translations.size(); ++k) {
//				const float frame_time = track.translations[k].time;
//
//				if (frame_time < 0.f || frame_time > rawAnimation->duration) {
//					console::warn("failed check duration translation track ", frame_time, ", ", rawAnimation->duration);
//				}
//
//				if (frame_time <= previous_time) {
//					console::warn("failed check previous translation track ", frame_time, ", ", previous_time);
//				}
//				previous_time = frame_time;
//			}
//		}
//
//		{
//			float previous_time = -1.f;
//			for (size_t k = 0; k < track.rotations.size(); ++k) {
//				const float frame_time = track.rotations[k].time;
//
//				if (frame_time < 0.f || frame_time > rawAnimation->duration) {
//					console::warn("failed check rotation track ", frame_time, ", ", rawAnimation->duration);
//				}
//
//				if (frame_time <= previous_time) {
//					console::warn("failed check rotation track ", frame_time, ", ", previous_time);
//				}
//				previous_time = frame_time;
//			}
//		}
//
//		{
//			float previous_time = -1.f;
//			for (size_t k = 0; k < track.scales.size(); ++k) {
//				const float frame_time = track.scales[k].time;
//
//				if (frame_time < 0.f || frame_time > rawAnimation->duration) {
//					console::warn("failed check scale track ", frame_time, ", ", rawAnimation->duration);
//				}
//
//				if (frame_time <= previous_time) {
//					console::warn("failed check scale track ", frame_time, ", ", previous_time);
//				}
//				previous_time = frame_time;
//			}
//		}
//	  }
}

void Model::setCurrentAnimation(std::string name) {
	console::infop("setCurrentAnimation %s, %i", name.c_str(), animations.size());
	auto it = animations.find(name);

	if (it == animations.end()) {
		console::warnp("no animation found %s %i", name.c_str());
		return;
	}

	currentAnimation = it->second;
	console::info("now current animation: ", currentAnimation);
}

void Model::tickAnimationTime(float time)
{
	if (currentAnimation == nullptr) {
		return;
	}

	if (animationTime + time > currentAnimation->duration()) {
		animationTime = 0.0f;
	} else {
		animationTime+= time;
	}
}

void Model::processAnimation()
{
	if (currentAnimation == nullptr) {
		return;
	}

	ozz::animation::SamplingJob samplingJob;
	samplingJob.animation = currentAnimation;
	samplingJob.cache = cache_;
	samplingJob.time = animationTime;
	samplingJob.output = locals_;
	if (!samplingJob.Run()) {
		console::warn("job not start");
		return;
	}

	ozz::animation::LocalToModelJob ltmJob;
	ltmJob.skeleton = skeleton;
	ltmJob.input = locals_;
	ltmJob.output = bones_;
	ltmJob.Run();
}

ozz::Range<ozz::math::Float4x4> * Model::getBones()
{
	return &bones_;
}
