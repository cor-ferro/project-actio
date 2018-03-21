#ifndef GAME_COMPONENTS_STATE_H_
#define GAME_COMPONENTS_STATE_H_

#include <string>
#include <glm/glm.hpp>

namespace game {
    namespace components {
        struct State {
            State(bool frozen = false, bool hidden = false) 
                : frozen(frozen)
                , hidden(hidden)
                {}

            std::string name;
            bool frozen;
            bool hidden;
        };
    }
}

#endif