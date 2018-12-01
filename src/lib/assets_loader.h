//
// Created by demitriy on 7/22/18.
//

#ifndef ACTIO_LIB_ASSETS_LOADER_H
#define ACTIO_LIB_ASSETS_LOADER_H

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include "resource.h"
#include "path.h"

namespace assets {
    class Loader {
    public:
        explicit Loader(const std::string &entryPoint);

        explicit Loader(const Path &entryPoint);

        Resource *createResource(const Path &path);

    protected:
        std::string entryPoint;
    };
}

#endif //ACTIO_LIB_ASSETS_LOADER_H
