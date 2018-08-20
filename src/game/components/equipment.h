//
// Created by demitriy on 8/20/18.
//

#ifndef ACTIO_GAME_COMPONENTS_EQUIPMENT_H
#define ACTIO_GAME_COMPONENTS_EQUIPMENT_H

#include <unordered_set>
#include <entityx/entityx/entityx.h>

namespace game {
    namespace components {
        namespace ex = entityx;

        class Equipment {
        public:
            Equipment() = default;

            const entityx::Entity &getPrimaryItem() const {
                return primaryItem;
            }

            void setPrimaryItem(const entityx::Entity &primaryItem) {
                Equipment::primaryItem = primaryItem;
            }

            const entityx::Entity &getSecondaryItem() const {
                return secondaryItem;
            }

            void setSecondaryItem(const entityx::Entity &secondaryItem) {
                Equipment::secondaryItem = secondaryItem;
            }

            bool equipPrimaryItem() {
                return primaryItem.valid();
            }

            bool equipSecondaryItem() {
                return secondaryItem.valid();
            }

        private:
            ex::Entity primaryItem;
            ex::Entity secondaryItem;

            std::unordered_set<ex::Entity> items;
        };
    }
}

#endif //ACTIO_GAME_COMPONENTS_EQUIPMENT_H
