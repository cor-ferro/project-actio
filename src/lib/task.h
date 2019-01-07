#ifndef ACTIO_WORLD_TASKS_H
#define ACTIO_WORLD_TASKS_H

#include <map>
#include <string>

class Task {
public:
    class BaseContext {};

    Task() = default;

    virtual ~Task() = 0;

    virtual void onStart() = 0;

    virtual void onFinish() = 0;

    virtual void onFlush() = 0;

    virtual void onFail() = 0;
};

#endif //ACTIO_WORLD_TASKS_H
