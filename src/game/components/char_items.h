//
// Created by demitriy on 4/27/18.
//

#ifndef ACTIO_CHAR_ITEMS_H
#define ACTIO_CHAR_ITEMS_H

#include <unordered_set>
#include <entityx/entityx/entityx.h>
#include "../../lib/console.h"
#include "../../lib/types.h"
#include "weapon.h"

namespace game {
    namespace components {
        namespace ex = entityx;

        class CharItems {
        public:
            CharItems() = default;

            void add(ex::Entity entity) {
                items.insert(entity);
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

            void setActiveItem(ex::Entity entity) {
                activeItem = entity;
            }

            bool isSetActiveItem() {
                return activeItem.valid();
            }

            ex::Entity getActiveItem() {
                return activeItem;
            }

            void execActiveItem(vec3 mousePos) {
                if (isSetActiveItem()) {
                    ex::ComponentHandle<Weapon> weapon = components::get<Weapon>(activeItem);

                    if (weapon) {
                        weapon->fire(mousePos);
                    }
                }
            }

        private:
            std::unordered_set<ex::Entity> items;
            ex::Entity activeItem;
        };
    }
}

#endif //ACTIO_CHAR_ITEMS_H
