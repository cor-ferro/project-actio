#ifndef ACTIO_LIB_PROFILING_H
#define ACTIO_LIB_PROFILING_H

#include <string>
#include <map>
#include <chrono>

#ifdef PROFILE_ENABLED
    #define PROFILE(profiler, label, payload) profiler.timeStart(label); payload; profiler.timeEnd(label);
#else
    #define PROFILE(profiler, label, payload) payload;
#endif


namespace profiling {
    using namespace std;

    typedef double TimePrecision;
    typedef chrono::time_point<chrono::high_resolution_clock> TimePoint;

    enum Time {
        Nanoseconds,
        Microseconds,
        Milliseconds,
        Seconds,
        Minutes,
        Hours
    };

    class ProfileTimings {
    public:
        explicit ProfileTimings(bool enabled = true);

        ~ProfileTimings();

        void timeStart(std::string label);

        void timeEnd(std::string label);

        std::map<std::string, TimePrecision> getDurations(Time timeType);

        TimePrecision getDuration(std::string label, Time timeType);

        void reset();

        void clear();

        void enable();

        void disable();

    private:
        struct TimeMark {
            TimePoint start;
            TimePoint end;
        };

        bool enabled;

        std::map<std::string, TimeMark> timings;

        inline TimePoint timeNow();

        inline TimePrecision castDuration(chrono::duration<TimePrecision> &ds, Time timeType);
    };

    struct Mark {
        Mark(ProfileTimings &profiler, std::string label);

        ~Mark();

        Mark(Mark && mark);

    private:
        ProfileTimings &profiler;
        std::string &label;
    };

    Mark mark(ProfileTimings &profiler, std::string label);
}

#endif //ACTIO_LIB_PROFILING_H
