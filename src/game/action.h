//
// Created by demitriy on 11/7/18.
//

#ifndef ACTIO_GAME_ACTION_H
#define ACTIO_GAME_ACTION_H

#include "context.h"

namespace game {
    class Action {
    public:
        Action(Context& context) : m_context(context) {

        };

        ~Action() = default;

    private:
        Context& m_context;
    };
}

#endif //ACTIO_GAME_ACTION_H
