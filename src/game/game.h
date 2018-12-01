#ifndef GAME_H_
#define GAME_H_

#include <memory>
#include "../app/app.h"
#include "engine.h"

namespace game {
//    World* createWorld();

    std::shared_ptr<Engine> createEngine(App& app);
}

#endif