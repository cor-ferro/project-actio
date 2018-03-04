#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <unordered_map>
#include <assimp/scene.h>
#include <ozz/animation/runtime/skeleton.h>
#include <ozz/animation/runtime/animation.h>
#include <ozz/animation/offline/animation_builder.h>
#include <ozz/animation/offline/raw_animation.h>
#include <ozz/animation/runtime/sampling_job.h>
#include <ozz/base/maths/quaternion.h>
#include <ozz/base/maths/simd_math.h>
#include <ozz/base/maths/soa_transform.h>
#include <ozz/base/maths/vec_float.h>
#include <ozz/base/memory/allocator.h>
#include "../lib/types.h"

namespace animation {
    struct Animation {
        Animation(aiAnimation * assimpAnimation, ozz::animation::Skeleton * skeleton);
        ~Animation();

        const std::string getName();
        void tick(float time);

        const float getDuration();
        const float getCurrentTime();

        void setCurrentTime(float time);

        bool valid();

        ozz::Range<ozz::math::SoaTransform>& getLocals();
        ozz::Range<ozz::math::Float4x4>& getModels();
        ozz::animation::SamplingCache* getCache();
        ozz::animation::Animation* getAnimation();
        mat4* getBones();
    private:
        void setName(const char* newName);
        void setName(std::string newName);
        void setDuration(float newDuration);

        float duration = 0.0f;
        float currentTime = 0.0f;

        std::string name;

        ozz::Range<ozz::math::SoaTransform> locals_;
        ozz::Range<ozz::math::Float4x4> models_;
        ozz::animation::SamplingCache* cache_ = nullptr;
        ozz::animation::Animation* animation_ = nullptr;
    };
}

#endif
