//
// Created by demitriy on 7/22/18.
//

#ifndef ACTIO_LIB_ASSETS_RESOURCE_H
#define ACTIO_LIB_ASSETS_RESOURCE_H

#include <memory>
#include "path.h"

namespace assets {
    class Resource {
    public:
        typedef std::shared_ptr<std::string> Content;

        explicit Resource(Path path);

        virtual ~Resource() = default;

        void reset();

        Content get();

        const size_t getSize();

        virtual void load() = 0;

        bool isLoaded();

    protected:
        Path path;
        bool loaded = false;
        std::shared_ptr<std::string> data;
        size_t size = 0;
    };


    class FilesystemResource : public Resource {
    public:
        explicit FilesystemResource(Path path);

        ~FilesystemResource() override = default;

        void load();
    };


    class PackageResource : public Resource {
    public:
        explicit PackageResource(Path path);

        ~PackageResource() override = default;

        void load();
    };
}

#endif //ACTIO_LIB_ASSETS_RESOURCE_H
