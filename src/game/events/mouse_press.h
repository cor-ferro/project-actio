//
// Created by demitriy on 3/31/18.
//

#ifndef ACTIO_MOUSE_PRESS_H
#define ACTIO_MOUSE_PRESS_H

namespace game {
    namespace events {
        struct MousePress {
            MousePress(int button, int action, int mods)
                    : button(button)
                    , action(action)
                    , mods(mods) {}

            int button;
            int action;
            int mods;
        };
    }
}

#endif //ACTIO_MOUSE_PRESS_H
