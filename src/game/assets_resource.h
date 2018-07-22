//
// Created by demitriy on 7/22/18.
//

#ifndef ACTIO_ASSETS_RESOURCE_H
#define ACTIO_ASSETS_RESOURCE_H

#include "../lib/path.h"

namespace game {
    namespace assets {
        class Resource {
        public:
            explicit Resource(Path path);

            virtual ~Resource() = default;

            void reset();

            char *get();

            const size_t getSize();

            virtual void load() = 0;

            bool isLoaded();

        protected:
            Path path;
            bool loaded = false;
            char *data = nullptr;
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
}

#endif //ACTIO_ASSETS_RESOURCE_H
