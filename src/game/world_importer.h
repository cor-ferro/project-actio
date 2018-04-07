//
// Created by demitriy on 4/5/18.
//

#ifndef ACTIO_WORLD_IMPORTER_H
#define ACTIO_WORLD_IMPORTER_H

#include "world.h"
#include "../resources/file_resource.h"

namespace game {
    struct WorldImporter {
        explicit WorldImporter(World *world);

        void import(const Resource::File &file);

    private:
        void importFromIniFile(const Resource::File &file);

        World *world;
    };
}

#endif //ACTIO_WORLD_IMPORTER_H
