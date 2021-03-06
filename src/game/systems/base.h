#ifndef ACTIO_BASE_SYSTEM_H
#define ACTIO_BASE_SYSTEM_H

#include <entityx/entityx/config.h>
#include <stack>
#include <functional>

namespace entityx {
    class EntityManager;
    class EventManager;
}

namespace game {
    class World;
    class Context;

    namespace systems {
        namespace ex = entityx;

        class BaseSystem {
        public:
            explicit BaseSystem(Context& context);

            void start(ex::EntityManager& es, ex::EventManager& events, ex::TimeDelta dt);

        protected:
            template<class T>
            void processQueue(std::stack<T> &items, const std::function<void (T&)> &handler) {
                int i = 0;

                while (!items.empty()) {
                    T& item = items.top();

                    handler(item);

                    items.pop();

                    i++;

                    if (i > maxItemsPerQueue) {
                        break;
                    }
                }
            };

            void setMaxItemsPerQueue(int count);

            const int getMaxItemsPerQueue() const;

            Context& m_context;

        private:
            int maxItemsPerQueue = 50;
        };
    }
}

#endif //ACTIO_BASE_SYSTEM_H
