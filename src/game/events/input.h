//
// Created by demitriy on 5/5/18.
//

#ifndef ACTIO_EVENTS_INPUT_H
#define ACTIO_EVENTS_INPUT_H

namespace game {
    namespace events {
        struct InputSetHandler {
            explicit InputSetHandler(InputHandler *inputHandler, int place)
                    : inputHandler(inputHandler)
                    , place(place) {};

            InputHandler *inputHandler;
            int place;
        };

        struct Input {
            explicit Input() = default;
        };
    }
}

#endif //ACTIO_EVENTS_INPUT_H
