//
// Created by demitriy on 4/17/18.
//

#ifndef ACTIO_GAME_LIGHT_HELPER_SHOW_H
#define ACTIO_GAME_LIGHT_HELPER_SHOW_H

namespace game {
    namespace events {
        struct LightHelperShow {
            LightHelperShow(bool value) : value(value) {}

            bool value;
        };
    }
}

#endif //ACTIO_GAME_LIGHT_HELPER_SHOW_H
