#include <glm/glm.hpp>
#include "animation.h"
#include "../lib/console.h"

namespace animation {
    Animation::Animation(aiAnimation * assimpAnimation, ozz::animation::Skeleton * skeleton)
    {
        ozz::animation::offline::RawAnimation rawAnimation;

        setName(assimpAnimation->mName.C_Str());
        setDuration(assimpAnimation->mDuration / assimpAnimation->mTicksPerSecond);

        rawAnimation.duration = getDuration();
        rawAnimation.tracks.resize(skeleton->num_joints());
        rawAnimation.name = name.c_str();

        ozz::Range<const char* const> jointNames = skeleton->joint_names();

        std::unordered_map<std::string, aiNodeAnim*> animNodes;

        for (int j = 0; j < assimpAnimation->mNumChannels; j++) {
            aiNodeAnim * nodeAnim = assimpAnimation->mChannels[j];

            animNodes.insert({ std::string(nodeAnim->mNodeName.C_Str()), nodeAnim });
        }

        for (uint i = 0; i < rawAnimation.num_tracks(); ++i) {
            ozz::animation::offline::RawAnimation::JointTrack& track = rawAnimation.tracks[i];
            const char* jointName = jointNames[i];

            auto it = animNodes.find(std::string(jointName));
            if (it == animNodes.end()) {
                continue;
            }

            aiNodeAnim * nodeAnim = it->second;

            assert(strcmp(nodeAnim->mNodeName.C_Str(), jointName) == 0);
            assert(nodeAnim != nullptr);

            track.translations.resize(nodeAnim->mNumPositionKeys);
            track.rotations.resize(nodeAnim->mNumRotationKeys);
            track.scales.resize(nodeAnim->mNumScalingKeys);

            float previousTranslationTime = -1.0f;

            for (uint j = 0; j < nodeAnim->mNumPositionKeys; j++) {
                const aiVectorKey& mPositionKey = nodeAnim->mPositionKeys[j];
                const aiVector3D& mVec = mPositionKey.mValue;

                float time = static_cast<float>(mPositionKey.mTime / assimpAnimation->mTicksPerSecond);
                assert(time >= 0.0f && time <= rawAnimation.duration && "translation");
                assert(time >= previousTranslationTime);

                ozz::animation::offline::RawAnimation::TranslationKey tkey;
                tkey.time = time;
                tkey.value = ozz::math::Float3((float)mVec.x, (float)mVec.y, (float)mVec.z);

                track.translations[j] = tkey;

                previousTranslationTime = time;
            }

            for (uint j = 0; j < nodeAnim->mNumRotationKeys; j++) {
                const aiQuatKey& mRotationKey = nodeAnim->mRotationKeys[j];
                const aiQuaternion& mQuat = mRotationKey.mValue;

                float time = static_cast<float>(mRotationKey.mTime / assimpAnimation->mTicksPerSecond);
                assert(time >= 0.0f && time <= rawAnimation.duration && "rotate");

                ozz::animation::offline::RawAnimation::RotationKey rkey;
                rkey.time = time;
                rkey.value = ozz::math::Quaternion(static_cast<float>(mQuat.x), static_cast<float>(mQuat.y), static_cast<float>(mQuat.z), static_cast<float>(mQuat.w));

                track.rotations[j] = rkey;
            }

            for (uint j = 0; j < nodeAnim->mNumScalingKeys; j++) {
                const aiVectorKey& mScalingKey = nodeAnim->mScalingKeys[j];
                const aiVector3D& mVec = mScalingKey.mValue;

                float time = static_cast<float>(mScalingKey.mTime / assimpAnimation->mTicksPerSecond);
                assert(time >= 0.0f && time <= rawAnimation.duration && "scale");
                
                ozz::animation::offline::RawAnimation::ScaleKey skey;
                skey.time = time;
                skey.value = ozz::math::Float3((float)mVec.x, (float)mVec.y, (float)mVec.z);

                track.scales[j] = skey;
            }
        }

        ozz::animation::offline::AnimationBuilder animationBuilder;
        animation_ = animationBuilder(rawAnimation);

        if (!animation_) {
            console::warnp("failed create animation %s", rawAnimation.name.c_str());
            return;
        }

        console::infop("animation: %s", getName().c_str());

        ozz::memory::Allocator* allocator = ozz::memory::default_allocator();

        const int numSoaJoints = skeleton->num_soa_joints();
        const int numJoints = skeleton->num_joints();

        locals_ = allocator->AllocateRange<ozz::math::SoaTransform>(numSoaJoints);
        models_ = allocator->AllocateRange<ozz::math::Float4x4>(numJoints);
        cache_ = allocator->New<ozz::animation::SamplingCache>(numJoints);
    }

    Animation::~Animation()
    {
        if (animation_ != nullptr) {
            console::info("destroy animation ", name.c_str());

            ozz::memory::Allocator* allocator = ozz::memory::default_allocator();

            allocator->Delete(animation_);
            allocator->Deallocate(locals_);
            allocator->Deallocate(models_);
            allocator->Delete(cache_);

            animation_ = nullptr;
            cache_ = nullptr;
        }
    }

    const std::string Animation::getName()
    {
        return name;
    }

    void Animation::setName(const char* newName)
    {
        name = newName;
    }

    void Animation::setName(std::string newName)
    {
        name = newName;
    }

    const float Animation::getDuration()
    {
        return duration;
    }

    void Animation::setDuration(float newDuration)
    {
        duration = newDuration;
    }

    const float Animation::getCurrentTime()
    {
        return currentTime;
    }

    void Animation::setCurrentTime(float time)
    {
        currentTime = glm::clamp<float>(time, 0.0f, duration);
    }

    void Animation::tick(float time)
    {
        float newTime = currentTime + time;

        currentTime = newTime >= duration ? 0.0f : newTime;
    }

    bool Animation::valid()
    {
        return animation_ != nullptr;
    }

    ozz::Range<ozz::math::SoaTransform>& Animation::getLocals()
    {
        return locals_;
    }

    ozz::Range<ozz::math::Float4x4>& Animation::getModels()
    {
        return models_;
    }

    ozz::animation::SamplingCache* Animation::getCache()
    {
        return cache_;
    }

    ozz::animation::Animation* Animation::getAnimation()
    {
        return animation_;
    }

    mat4* Animation::getBones()
    {
        void* vModels = static_cast<void*>(&models_[0].cols[0][0]);
        mat4* bones = static_cast<mat4*>(vModels);

        return bones;
    }
}
