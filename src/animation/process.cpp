#include <ozz/animation/runtime/local_to_model_job.h>
#include <ozz/animation/runtime/sampling_job.h>
#include <ozz/animation/runtime/blending_job.h>
#include "process.h"
#include "../lib/console.h"

namespace animation {
    bool sampleAnimation(Animation *animation) {
        // Setup sampling job.
        ozz::animation::SamplingJob sampling_job;
        sampling_job.animation = animation->getAnimation();
        sampling_job.cache = animation->getCache();
        sampling_job.time = animation->getCurrentTime();
        sampling_job.output = animation->getLocals();

        // Samples animation.
        if (!sampling_job.Run()) {
            return false;
        }

        return true;
    }

    void process(Animation *animation, ozz::animation::Skeleton *skeleton) {
        if (sampleAnimation(animation)) {
            ozz::animation::LocalToModelJob ltmJob;
            ltmJob.skeleton = skeleton;
            ltmJob.input = animation->getLocals();
            ltmJob.output = animation->getModels();
            ltmJob.Run();
        }
    }

    void process(animation::AnimationHandler &animationSamplers, animation::Handler *handler) {
        const std::vector<Sampler> &samplers = animationSamplers.getSamplers();

        const size_t countSamplers = animationSamplers.countActiveSamplers();

        // @todo: change to smart pointer
        auto *layersPtr = new ozz::animation::BlendingJob::Layer[countSamplers];
        auto layers = ozz::Range<ozz::animation::BlendingJob::Layer>(layersPtr, countSamplers);

        for (auto &sampler : samplers) {
            if (sampler.isCanUsed()) {
                sampleAnimation(sampler.animation);
            }
        }

        size_t i = 0;
        for (auto &sampler : samplers) {
            if (sampler.isCanUsed()) {
                layers[i].transform = sampler.animation->getLocals();
                layers[i].weight = sampler.weight;
                i++;
            }
        }

        ozz::animation::BlendingJob blend_job;
        blend_job.threshold = 1.0f;
        blend_job.layers = layers;
        blend_job.bind_pose = handler->skeleton->bind_pose();
        blend_job.output = handler->locals;

        if (!blend_job.Run()) {
            delete[] layersPtr;
            return;
        }

        ozz::animation::LocalToModelJob ltm_job;
        ltm_job.skeleton = handler->skeleton;
        ltm_job.input = handler->locals;
        ltm_job.output = handler->models;

        if (!ltm_job.Run()) {
            delete[] layersPtr;
            return;
        }

        delete[] layersPtr;
    }
}