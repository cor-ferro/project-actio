#ifndef ACTIO_SKIN_H
#define ACTIO_SKIN_H

#include <unordered_map>
#include <ozz/base/memory/allocator.h>
#include <ozz/animation/runtime/skeleton.h>
#include <ozz/animation/offline/raw_skeleton.h>
#include <ozz/animation/offline/skeleton_builder.h>
#include "../../animation/animation.h"
#include "../../core/mesh.h"
#include "../../animation/process.h"

namespace game {
    namespace components {
        using ozz::animation::offline::RawSkeleton;

        struct Skin {
            Skin(const Resource::Assimp *assimpResource) {
                aiScene const *scene = assimpResource->getScene();

                const aiNode *rootNode = assimpResource->getRootNode();

                inverseMat = libAi::toNativeType(rootNode->mTransformation);
                inverseMat = glm::inverse(inverseMat);

                if (scene->mNumAnimations > 0) {
                    ozz::animation::offline::SkeletonBuilder skeletonBuilder;

                    RawSkeleton rawSkeleton;
                    createSkeleton(assimpResource, &rawSkeleton);

                    skeleton = skeletonBuilder(rawSkeleton);

                    for (int i = 0; i < scene->mNumAnimations; i++) {
                        auto *animation = new animation::Animation(scene->mAnimations[i], skeleton);

                        if (animation->valid()) {
                            animations.insert({animation->getName(), animation});
                        }
                    }

                    ozz::Range<const char* const> jointNames = skeleton->joint_names();

                    for (uint jIndex = 0; jIndex < jointNames.Count(); jIndex++) {
//                        console::info("joint: %s, %i", jointNames[jIndex], jIndex);
                    }
                }
            }

            ~Skin() {
                currentAnimation = nullptr;

                if (skeleton != nullptr) {
                    ozz::memory::Allocator *allocator = ozz::memory::default_allocator();

                    for (auto &pair : animations) {
                        delete pair.second;
                        pair.second = nullptr;
                    }

                    animations.erase(animations.begin(), animations.end());
                    currentAnimation = nullptr;

                    allocator->Delete(skeleton);
                    skeleton = nullptr;
                }
            }

            void processAnimation(double dt) {
                assert(currentAnimation != nullptr);

                currentAnimation->tick(static_cast<float>(dt / 1000.0));

                processAnimation();
            }

            void processAnimation() {
                assert(currentAnimation != nullptr);

                animation::process(currentAnimation, skeleton);
            }

            mat4 *getTransforms() {
                assert(currentAnimation != nullptr);

                return currentAnimation->getBones();
            }

            int getNumBones() {
                return skeleton->num_joints();
            }

            std::unordered_map<std::string, uint> getNodeIndexes() {
                std::unordered_map<std::string, uint> map;

                ozz::Range<const char* const> jointNames = skeleton->joint_names();

                for (uint jIndex = 0; jIndex < jointNames.Count(); jIndex++) {
                    map.insert({ std::string(jointNames[jIndex]), jIndex });
                }

                return map;
            };

            bool canProcessAnimation() {
                return currentAnimation != nullptr;
            }

            void setCurrentAnimation(std::string animationName) {
                auto it = animations.find(animationName);

                if (it != animations.end()) {
                    currentAnimation = it->second;
                }
            }

            mat4 inverseMat;

        private:
            void createSkeleton(const Resource::Assimp *assimpResource, RawSkeleton *rawSkeleton) {
                const aiNode *rootNode = assimpResource->getRootNode();

                ozz::memory::Allocator *allocator = ozz::memory::default_allocator();

                rawSkeleton->roots.resize(1);

                RawSkeleton::Joint *root = &rawSkeleton->roots[0];
                root->name = rootNode->mName.C_Str();

                aiMatrix4x4 mat = rootNode->mTransformation;
                aiVector3D translation, scale;
                aiQuaternion rotation;
                mat.Decompose(scale, rotation, translation);

                createJoints(rootNode, root);
            }

            void createJoints(const aiNode *node, RawSkeleton::Joint *joint) {
                joint->children.resize(node->mNumChildren);


                for (uint i = 0; i < node->mNumChildren; i++) {
                    RawSkeleton::Joint *childJoint = &joint->children[i];
                    childJoint->name = node->mChildren[i]->mName.C_Str();

                    createJoints(node->mChildren[i], childJoint);
                }
            }

            ozz::animation::Skeleton *skeleton = nullptr;
            animation::Animation *currentAnimation = nullptr;
            std::unordered_map<std::string, animation::Animation *> animations;
        };
    }
}

#endif //ACTIO_SKIN_H
