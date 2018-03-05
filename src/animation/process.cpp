#include <ozz/animation/runtime/local_to_model_job.h>
#include <ozz/animation/runtime/sampling_job.h>
#include "process.h"
#include "../lib/console.h"

namespace animation {
    void process(Animation * animation, ozz::animation::Skeleton * skeleton) {
        ozz::animation::SamplingJob samplingJob;
        samplingJob.animation = animation->getAnimation();
        samplingJob.cache = animation->getCache();
        samplingJob.time = animation->getCurrentTime();
        samplingJob.output = animation->getLocals();
        if (!samplingJob.Run()) {
            console::warn("animation job not start");
            return;
        }

        ozz::animation::LocalToModelJob ltmJob;
        ltmJob.skeleton = skeleton;
        ltmJob.input = animation->getLocals();
        ltmJob.output = animation->getModels();
        ltmJob.Run();
    }
}