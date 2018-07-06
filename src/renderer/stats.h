#ifndef STATS_H_
#define STATS_H_

#include <boost/chrono/chrono.hpp>
#include <boost/timer/timer.hpp>
#include "../lib/console.h"
#include "../core/geometry.h"

namespace renderer {

    typedef boost::chrono::system_clock SystemClock;

    struct Stats {
        Stats();

        void resetFrame();

        void startTime();

        void updateTime();

        void increaseDrawCall();

        size_t getDrawCalls() const;

        double getFps() const;
        double getMsFrame() const;

        double fps = 0.0;
        double msFrame = 0.0;

    private:
        SystemClock::time_point timeTick1_;
        SystemClock::time_point timeTick2_;
        SystemClock::time_point timeFrame_;
        double fpsCounter;
        size_t drawCalls = 0;
    };

}

#endif /* STATS_H_ */
