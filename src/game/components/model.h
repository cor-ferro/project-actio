//
// Created by demitriy on 3/24/18.
//

#ifndef ACTIO_GAME_COMPONENTS_MODEL_H
#define ACTIO_GAME_COMPONENTS_MODEL_H

#include "../../core/model.h"
#include "../../core/mesh.h"

namespace game {
    namespace components {
        struct Model : ::Model {
            Model() : ::Model() {}
            Model(Mesh *mesh) : ::Model(mesh) {}
        };
    }
}

#endif //ACTIO_GAME_COMPONENTS_MODEL_H
