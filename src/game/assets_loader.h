//
// Created by demitriy on 7/22/18.
//

#ifndef ACTIO_ASSETS_LOADER_H
#define ACTIO_ASSETS_LOADER_H

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include "assets_resource.h"
#include "../lib/path.h"

namespace game {
    namespace assets {
        class Loader {
        public:
            explicit Loader(const std::string &entryPoint);

            Resource *load(const Path &path);

        protected:
            std::string entryPoint;
        };
    }
}

#endif //ACTIO_ASSETS_LOADER_H
