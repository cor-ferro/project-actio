//
// Created by demitriy on 7/22/18.
//

#ifndef ACTIO_FS_LOADER_H
#define ACTIO_FS_LOADER_H

#include <string>
#include <boost/filesystem/path.hpp>
#include "path.h"

namespace loader {
    typedef char byte;

    class Base {
    public:
        explicit Base(Path rootPath);
        virtual ~Base() = default;

        virtual byte *load(Path name) = 0;

    protected:
        Path rootPath;
    };

    class Filesystem : Base {
    public:
        explicit Filesystem(Path rootPath);
        ~Filesystem() = default;

        byte *load(Path name) override;
    };

    class Package : Base {
    public:
        explicit Package(Path rootPath);
        ~Package() = default;
        byte *load(Path name) override;
    };
}

#endif //ACTIO_FS_LOADER_H
