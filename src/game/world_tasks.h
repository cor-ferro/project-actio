//
// Created by demitriy on 9/29/18.
//

#ifndef ACTIO_WORLD_TASKS_H
#define ACTIO_WORLD_TASKS_H

namespace game {
    class World;

    class WorldTaskContext {
    public:
        explicit WorldTaskContext(World *world);
        ~WorldTaskContext();

        void *getData();
        void setData(void *data);

        World *getWorld();

    private:
        World *world = nullptr;
        void *data = nullptr;
    };

    class WorldTask {
    public:
        explicit WorldTask(WorldTaskContext *context);

        ~WorldTask();

        virtual void onStart() = 0;
        virtual void onFinish() = 0;
        virtual void onFlush() = 0;

    protected:
        WorldTaskContext *context = nullptr;
    };

    class WorldModelLoadTask : public WorldTask {
    public:
        explicit WorldModelLoadTask(WorldTaskContext *context);

        ~WorldModelLoadTask() = default;

        void onStart() override;

        void onFinish() override;

        void onFlush() override;
    };
}

#endif //ACTIO_WORLD_TASKS_H
