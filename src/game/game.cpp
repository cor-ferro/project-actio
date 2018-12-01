#include "game.h"

namespace game {
    std::shared_ptr<Engine> createEngine(App& app) {
        std::shared_ptr<Engine> ptr(new Engine(app));

        return ptr;
    }
}