//
// Created by demitriy on 3/31/18.
//

#ifndef ACTIO_KEY_PRESS_H
#define ACTIO_KEY_PRESS_H

namespace game {
    namespace events {
        struct KeyPress {
            KeyPress(int key, int scancode, int action, int mods)
                    : key(key)
                    , scancode(scancode)
                    , action(action)
                    , mods(mods) {}

            int key;
            int scancode;
            int action;
            int mods;
        };
    }
}

#endif //ACTIO_KEY_PRESS_H
