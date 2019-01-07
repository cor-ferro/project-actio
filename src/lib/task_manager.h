#ifndef ACTIO_TASKS_MANAGER_H
#define ACTIO_TASKS_MANAGER_H

#include <queue>
#include <map>
#include <thread>
#include <cassert>
#include <exception>

#include "task.h"
#include "console.h"

class TasksManager {
public:
    ~TasksManager() {
        if (!threads.empty()) {
            console::warn("some threads not finished");
        }
    }

    void addNewTask(Task *task) {
        tasks.push(task);
    }

    bool hasCompleted() {
        return !completedTasks.empty();
    }

    std::queue<Task*>& getCompleted() {
        return completedTasks;
    }

    void process() {
        if (tasks.empty() || processedTask != nullptr) {
            return;
        }

        Task *task = tasks.front();
        tasks.pop();

        processedTask = task;

        std::thread t([this]() {
            std::thread::id tid = std::this_thread::get_id();
            console::info("make task thread: %i", tid);

            this->threads.insert({tid, true});

            assert(this->processedTask != nullptr);

            try {
                this->processedTask->onStart();
                this->processedTask->onFinish();
                completedTasks.push(this->processedTask);
            } catch (std::exception &e) {
                this->processedTask->onFail();
                console::info("fail to execute task");
            }

            this->processedTask = nullptr;

            auto it = this->threads.find(tid);

            if (it != this->threads.end()) {
                this->threads.erase(it);
                console::info("complete thread: %i", tid);
            } else {
                console::warn("unable found the thread %i", tid);
            }
        });

        t.detach();
    }

private:
    Task *processedTask = nullptr;
    std::queue<Task *> tasks;
    std::queue<Task *> completedTasks;

    std::map<std::thread::id, bool> threads;
};

#endif //ACTIO_TASKS_MANAGER_H
