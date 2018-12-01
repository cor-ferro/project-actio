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
#include "../lib/console.h"

namespace animation {
    struct Handler {
        explicit Handler(ozz::animation::Skeleton *skeleton) : skeleton(skeleton) {
            ozz::memory::Allocator *allocator = ozz::memory::default_allocator();

            locals = allocator->AllocateRange<ozz::math::SoaTransform>(skeleton->num_soa_joints());
            models = allocator->AllocateRange<ozz::math::Float4x4>(skeleton->num_joints());
        }

        ~Handler() {
            ozz::memory::Allocator *allocator = ozz::memory::default_allocator();
            allocator->Deallocate(locals);
            allocator->Deallocate(models);
        }

        mat4 *getTransforms() {
            void *vModels = static_cast<void *>(&models[0].cols[0][0]);
            mat4 *transforms = static_cast<mat4 *>(vModels);

            return transforms;
        }

        ozz::Range<ozz::math::SoaTransform> locals;
        ozz::Range<ozz::math::Float4x4> models;
        ozz::animation::Skeleton *skeleton;
    };

    struct Animation {
        Animation(aiAnimation *assimpAnimation, ozz::animation::Skeleton *skeleton);

        ~Animation();

        const std::string getName();

        void tick(float time);

        const float getDuration();

        const float getCurrentTime();

        void setCurrentTime(float time);

        bool valid();

        ozz::Range<ozz::math::SoaTransform> &getLocals();

        ozz::Range<ozz::math::Float4x4> &getModels();

        ozz::animation::SamplingCache *getCache();

        ozz::animation::Animation *getAnimation();

        mat4 *getTransforms();

    private:
        void setName(const char *newName);

        void setName(std::string newName);

        void setDuration(float newDuration);

        float duration = 0.0f;
        float currentTime = 0.0f;

        std::string name;

        ozz::Range<ozz::math::SoaTransform> locals_;
        ozz::Range<ozz::math::Float4x4> models_;
        ozz::animation::SamplingCache *cache_ = nullptr;
        ozz::animation::Animation *animation_ = nullptr;
    };

    struct Sampler {
        Sampler() : animation(nullptr) {};

        explicit Sampler(Animation *animation) : animation(animation) {}

        void tick(double dt) const {
            animation->tick(static_cast<float>(dt));
        }

        void tick(float dt) const {
            animation->tick(dt);
        }

        bool isCanUsed() const {
            return animation != nullptr;
        }

        Animation *animation = nullptr;
        float weight = 0.0f;
        float initialWeight = 0.0f;
        float targetWeight = 0.0f;
        float step = 0.0f;
    };

    class AnimationHandler {
    public:
        void update(double dt) {
//            tick(static_cast<float>(dt));
            reduce();
        }

        void update(float dt) {
//            tick(dt);
            reduce();
        }

        int add(Animation *animation) {
            auto newIndex = static_cast<int>(samplers.size());

            samplers.emplace_back(Sampler(animation));

            return newIndex;
        }

        void set(int index, Animation *animation) {
            if (samplers.size() < index + 1) {
                samplers.resize(index + 1);
            }

            samplers[index].animation = animation;
        }

        float weight(int index) {
            assert(samplers.size() >= index + 1);

            return samplers[index].weight;
        }

        void setWeight(int index, float value) {
            assert(samplers.size() >= index + 1);

            samplers[index].weight = value;
        }

        void strive(int index, float newWeight, float newStep) {
            if (index == -1) return;

            assert(samplers.size() >= index + 1);

            samplers[index].targetWeight = glm::clamp(newWeight, 0.0f, 1.0f);
            samplers[index].initialWeight = samplers[index].weight;
            samplers[index].step = newStep;
        }

        const std::vector<Sampler> &getSamplers() {
            return samplers;
        }

        const size_t countActiveSamplers() {
            size_t size = 0;

            for (auto &sampler : samplers) {
                if (sampler.isCanUsed()) size++;
            }

            return size;
        }

        const size_t countSamplers() {
            return samplers.size();
        }
    private:
        void tick(float dt) {
            for (auto &sampler : samplers) {
                if (sampler.weight > 0.0f) {
                    sampler.animation->tick(dt);
                }
            }
        }

        void reduce() {
            for (auto &sampler : samplers) {
                if (sampler.targetWeight > sampler.weight) {
                    sampler.weight+= sampler.step;
                    sampler.weight = glm::clamp(sampler.weight, sampler.initialWeight, sampler.targetWeight);
                } else if (sampler.targetWeight < sampler.weight) {
                    sampler.weight-= sampler.step;
                    sampler.weight = glm::clamp(sampler.weight, sampler.targetWeight, sampler.initialWeight);
                }
            }
        }

        std::vector<Sampler> samplers;
    };
}

#endif
