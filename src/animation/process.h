#ifndef ANIMATION_PROCESS_H
#define ANIMATION_PROCESS_H

#include "animation.h"
#include <ozz/animation/runtime/skeleton.h>

namespace animation {
    void process(Animation * animation, ozz::animation::Skeleton * skeleton);
    void process(animation::AnimationHandler &animationSamplers, animation::Handler *handler);
}

#endif