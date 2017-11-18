#include "model.h"

// #define THREAD_INIT_AI_

ModelNode::ModelNode() 
	: name("")
	{}

ModelNode::ModelNode(aiNode * node) 
	: name(std::string(node->mName.C_Str()))
{
	meshes.reserve(node->mNumMeshes);
	children.reserve(node->mNumChildren);

	transformation = aiMatToMat(node->mTransformation);
}

void ModelNode::addMesh(Mesh * mesh)
{
	meshes.push_back(std::shared_ptr<Mesh>(mesh));
}

void ModelNode::addNode(ModelNode * node)
{
	children.push_back(std::shared_ptr<ModelNode>(node));
}

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
	nodes_ = other.nodes_;
	animations_ = other.animations_;
}

Model::Model(Mesh * mesh)
{
	allocMeshes(1);
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

	this->GlobalInverseTransform = aiMatToMat(scene->mRootNode->mTransformation);
	this->GlobalInverseTransform = glm::inverse(GlobalInverseTransform);

	if (scene->mNumAnimations > 0) {
		console::info("animations:", scene->mNumAnimations);
		for (int i = 0; i < scene->mNumAnimations; i++)
		{
			const Animation * animation = new Animation(scene->mAnimations[i]);
			addAnimation(animation);

			/*const aiAnimation * animation = scene->mAnimations[i];
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
			}*/
		}

		if (animations_.size() > 0) {
			setCurrentAnimation("idle");
		}
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
	meshes_.clear();
	meshes_.reserve(count);
}

void Model::freeMeshes()
{
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

	// boost::thread_group tg;
	// unsigned int numMeshes = assimpResource->scene->mNumMeshes;
	// for (unsigned int i = 0; i < numMeshes; i++) {
	// 	#ifndef THREAD_INIT_AI
	// 	processMesh(assimpResource->scene->mMeshes[i], assimpResource);
	// 	#endif
	// 	#ifdef THREAD_INIT_AI
	// 	tg.add_thread(
	// 		new boost::thread(
	// 			&Model::processMesh, this, assimpResource->scene->mMeshes[i], assimpResource
	// 		)
	// 	);
	// 	#endif
	// }

	// #ifdef THREAD_INIT_AI
	// tg.join_all();
	// #endif

	rootNode_ = new ModelNode(assimpResource->scene->mRootNode);

	processNode(assimpResource->scene->mRootNode, rootNode_, assimpResource);

	for(auto mesh = meshes_.begin(); mesh != meshes_.end(); mesh++)
	{
		(*mesh)->setup();
		(*mesh)->material.setupTextures();
	}
}

void Model::addMesh(Mesh * mesh)
{
	meshes_.push_back(std::shared_ptr<Mesh>(mesh));
}

void Model::addNode(ModelNode * node)
{
	nodes_.insert(
		std::pair<std::string, std::shared_ptr<ModelNode>>(node->name, std::shared_ptr<ModelNode>(node))
	);
}

void Model::processNode(aiNode * node, ModelNode * modelNode, const Resource::Assimp * assimpResource)
{
	addNode(modelNode);

	// console::info("======= node", modelNode->name, node->mNumMeshes);

	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		unsigned int meshIndex = node->mMeshes[i];
		aiMesh * mesh = assimpResource->scene->mMeshes[meshIndex];
		Mesh * modelMesh = new Mesh(mesh, assimpResource);
		addMesh(modelMesh);
		modelNode->addMesh(modelMesh);
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		aiNode * childNode = node->mChildren[i];
		ModelNode * childModelNode = new ModelNode(childNode);
		modelNode->addNode(childModelNode);
		processNode(childNode, childModelNode, assimpResource);
	}
}

const ModelMeshes& Model::getMeshes()
{
	return meshes_;
}

Mesh * Model::getFirstMesh()
{
	return meshes_.at(0).get();
}

void Model::scale(vec3 scale) {
	for(auto mesh = meshes_.begin(); mesh != meshes_.end(); mesh++)
	{
		(*mesh)->setScale(scale);
	}
}

void Model::rotate(vec3 rotate, float angle) {
	for(auto mesh = meshes_.begin(); mesh != meshes_.end(); mesh++)
	{
		(*mesh)->rotate(rotate, angle);
	}
}

void Model::setCurrentAnimation(std::string animationName)
{
	const Animation * animation = getAnimation(animationName);

	if (animation == nullptr) {
		currentAnimation.setAnimation(nullptr);
		console::warn("set current animation failed. animation not found: ", animationName);
		return;
	}

	currentAnimation.setAnimation(animation);
}

void Model::addAnimation(const Animation * animation)
{
	std::string animationName = animation->getName();
	std::shared_ptr<const Animation> animationPtr(animation);

	animations_.insert(
		std::pair<std::string, std::shared_ptr<const Animation>>(animationName, animationPtr)
	);
}

const Animation * const Model::getAnimation(const std::string& name)
{
	auto it = animations_.find(name);
	
	if (it == animations_.end()) {
		return nullptr;
	}

	return it->second.get();
}

void Model::processCurrentAnimation()
{
	processAnimation(currentAnimation);
}

void Model::processAnimation(AnimationProcess& animationProcess)
{
	const Animation * animation = animationProcess.getAnimation();

	if (animation == nullptr) {
		console::warn("not found animation");
		return;
	}

	double tick = animationProcess.getCurrentTick();

	// console::info("tick ", tick);

	// console::info("process animation", animation->getName());
	mat4 rootTransform(1.0);
	processNodeAnimation(rootNode_, animation, rootTransform, tick);
	// рекурсивно проходимся по нодам модели, начиная с рутовой
	// в объекте анимации находим анимацию ноды по названию самой ноды
	// в ноде анимации вычисляем интерполированные значения на основе времени для вращения/положения/скейла
	// нужно установить значения костям соответствующим ноде анимации
	// в ноде находим меши и кости равным названию самой ноды, и выставить значения матрицы
}

// @todo: оптимизировать обход по иерархии нод
void Model::processNodeAnimation(ModelNode * node, const Animation * animation, mat4& rootTransform, double tick)
{
	const NodeAnimation * nodeAnim = animation->findNode(node->name);

	if (nodeAnim != nullptr) {
		const AnimKeyPosition& positionKey = nodeAnim->findPosition(tick);
		const AnimKeyRotation& rotateKey = nodeAnim->findRotation(tick);
		const AnimKeyScale& scaleKey = nodeAnim->findScale(tick);

		mat4 nodeTransform(1.0);

		mat4 position = glm::translate(mat4(1.0), positionKey.value);
		mat4 rotate = glm::toMat4(rotateKey.value);
		mat4 scale = glm::scale(mat4(1.0), scaleKey.value);

		nodeTransform = position * rotate * scale;
		mat4 newRootTransform(GlobalInverseTransform * rootTransform * nodeTransform);

		// @todo: мы проходимся по всем мешам в поиске нужных костей - оптимизировать обход
		for (auto it = meshes_.begin(); it != meshes_.end(); it++) {
			auto boneIt = (*it)->bones.find(node->name);

			if (boneIt != (*it)->bones.end()) {
				mat4 finalTransform = newRootTransform * boneIt->second.getOffset();
				boneIt->second.setTransform(finalTransform);
			}
		}
		
		for (auto it = node->children.begin(); it != node->children.end(); it++) {
			processNodeAnimation((*it).get(), animation, newRootTransform, tick);
		}
	} else {
		for (auto it = node->children.begin(); it != node->children.end(); it++) {
			processNodeAnimation((*it).get(), animation, rootTransform, tick);
		}
	}
}