#include "profiling.h"
#include "console.h"

void inta() {
    console::info("a");
}

namespace profiling {
    ProfileTimings::ProfileTimings(bool enabled) : enabled(enabled) {};

    ProfileTimings::~ProfileTimings() {
        console::info("profiler destroyed1");
    }

    void ProfileTimings::timeStart(std::string label) {
        if (!enabled) return;

        TimeMark &mark = timings[label];

        mark.start = timeNow();
    }

    void ProfileTimings::timeEnd(std::string label) {
        if (!enabled) return;

        TimeMark &mark = timings[label];

        mark.end = timeNow();
    }

    std::map<std::string, TimePrecision> ProfileTimings::getDurations(Time timeType) {
        std::map<std::string, TimePrecision> results;

        for (auto &it : timings) {
            TimeMark &mark = it.second;

            std::chrono::duration<TimePrecision> ds = mark.end - mark.start;
            TimePrecision t = castDuration(ds, timeType);

            results.insert({it.first, t});
        }

        return results;
    };

    TimePrecision ProfileTimings::getDuration(std::string label, Time timeType) {
        TimeMark &mark = timings[label];

        chrono::duration<TimePrecision> ds = mark.end - mark.start;

        return castDuration(ds, timeType);
    }

    void ProfileTimings::reset() {
        for (auto &it : timings) {
            it.second.start = timeNow();
            it.second.end = timeNow();
        }
    }

    void ProfileTimings::clear() {
        timings.clear();
    }

    void ProfileTimings::enable() {
        enabled = true;
    }

    void ProfileTimings::disable() {
        enabled = false;
    }

    TimePoint ProfileTimings::timeNow() {
        return chrono::high_resolution_clock::now();
    }

    TimePrecision ProfileTimings::castDuration(chrono::duration<TimePrecision> &ds, Time timeType) {
        switch (timeType) {
            case Nanoseconds:
                return chrono::duration_cast<chrono::nanoseconds>(ds).count();
            case Microseconds:
                return chrono::duration_cast<chrono::microseconds>(ds).count();
            case Milliseconds:
                return chrono::duration_cast<chrono::milliseconds>(ds).count();
            case Seconds:
                return chrono::duration_cast<chrono::seconds>(ds).count();
            case Minutes:
                return chrono::duration_cast<chrono::minutes>(ds).count();
            case Hours:
                return chrono::duration_cast<chrono::hours>(ds).count();
            default:
                return static_cast<TimePrecision>(0);
        }
    }

    Mark::Mark(ProfileTimings &profiler, std::string label) : profiler(profiler), label(label) {
        profiler.timeStart(label);
    }

    Mark::~Mark() {
        profiler.timeEnd(label);
    }

    Mark::Mark(Mark && mark) : profiler(mark.profiler), label(mark.label) {}

    Mark mark(ProfileTimings &profiler, std::string label) {
        return std::move(Mark(profiler, label));
    }
}
