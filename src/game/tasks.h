//
// Created by demitriy on 9/29/18.
//

#ifndef ACTIO_WORLD_TASKS_H
#define ACTIO_WORLD_TASKS_H

#include "context.h"

namespace game {
    class World;

    class TaskContext {
    public:
        explicit TaskContext(Context& context);

        ~TaskContext() = default;

        void *getData();

        void setData(void *data);

        Context& getContext();

    private:
        Context& m_context;
        void *data = nullptr;
    };

    class Task {
    public:
        explicit Task(TaskContext &taskContext);

        ~Task() = default;

        virtual void onStart() = 0;

        virtual void onFinish() = 0;

        virtual void onFlush() = 0;

    protected:
        TaskContext &m_taskContext;
    };

    class TaskLoadModel : public Task {
    public:
        explicit TaskLoadModel(TaskContext &taskContext);

        ~TaskLoadModel() = default;

        void onStart() override;

        void onFinish() override;

        void onFlush() override;
    };
}

#endif //ACTIO_WORLD_TASKS_H
