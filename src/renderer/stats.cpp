#include "./stats.h"
#include <iostream>
#include <iomanip>

namespace renderer {

    Stats::Stats() {
        timeTick1_ = timeTick2_ = timeFrame_ = SystemClock::now();
    }

    void Stats::resetFrame() {
        drawCalls = 0;
    }

    void Stats::startTime() {
        timeTick1_ = boost::chrono::system_clock::now();
    }

    void Stats::updateTime() {
        fpsCounter++;
        timeTick2_ = boost::chrono::system_clock::now();

        boost::chrono::microseconds tick = boost::chrono::duration_cast<boost::chrono::microseconds>(
                timeTick2_ - timeTick1_);
        boost::chrono::milliseconds frameTick = boost::chrono::duration_cast<boost::chrono::milliseconds>(
                timeTick2_ - timeFrame_);

        if (frameTick.count() >= 1000.0) {
            timeFrame_ = timeTick2_;
            fps = fpsCounter;
            fpsCounter = 0;
        }

        msFrame = tick.count() / 1000.0;
    }

    void Stats::increaseDrawCall() {
        drawCalls++;
    }

    size_t Stats::getDrawCalls() const {
        return drawCalls;
    }

    double Stats::getFps() const {
        return fps;
    };
    double Stats::getMsFrame() const {
        return msFrame;
    };

}


