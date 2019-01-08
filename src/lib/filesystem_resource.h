//
// Created by demitriy on 30.11.18.
//

#ifndef ACTIO_FILESYSTEM_RESOURCE_H
#define ACTIO_FILESYSTEM_RESOURCE_H

#include "resource.h"
#include "path.h"
#include "data.h"

class FilesystemResource : public Resource {
public:
    explicit FilesystemResource(const Path &path);

    ~FilesystemResource() override = default;

    void load() override;

    const Path &getPath() const;

    const void *const get() const override;

private:
    Path path;
    mutable Data data;
};

#endif //ACTIO_FILESYSTEM_RESOURCE_H
