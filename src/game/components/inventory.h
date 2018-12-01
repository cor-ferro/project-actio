//
// Created by demitriy on 4/27/18.
//

#ifndef ACTIO_CHAR_ITEMS_H
#define ACTIO_CHAR_ITEMS_H

#include <unordered_set>
#include <entityx/entityx/entityx.h>
#include "../../lib/console.h"
#include "../../lib/types.h"

namespace game {
    namespace components {
        namespace ex = entityx;

        class Inventory {
        public:
            Inventory() = default;

            bool add(ex::Entity entity) {
                if (items.size() + 1 >= maxItems) {
                    return false;
                }

                items.insert(entity);

                return true;
            }

            bool has(ex::Entity entity) {
                auto it = items.find(entity);

                return it != items.end();
            }

            void remove(ex::Entity entity) {
                auto it = items.find(entity);

                if (it != items.end()) {
                    items.erase(it);
                }
            }

        private:
            std::unordered_set<ex::Entity> items;
            int maxItems = 10;
        };
    }
}

#endif //ACTIO_CHAR_ITEMS_H
