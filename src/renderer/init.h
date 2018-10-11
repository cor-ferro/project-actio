//
// Created by demitriy on 10/6/18.
//

#ifndef ACTIO_INIT_H
#define ACTIO_INIT_H

#include <boost/filesystem.hpp>
#include <vector>
#include <list>
#include <map>

#include "./opengl/renderer.h"
#include "params.h"
#include "raw_program.h"
#include "../lib/path.h"
#include "../lib/utils.h"

namespace renderer {
    namespace fs = boost::filesystem;

    Renderer *create(Params &params);

    void destroy(Renderer *renderer);

    void loadPrograms(std::map<std::string, RawProgram> &programs);
}

#endif //ACTIO_INIT_H
