//
// Created by demitriy on 30.11.18.
//

#ifndef ACTIO_LIB_PACKAGE_RESOURCE_H
#define ACTIO_LIB_PACKAGE_RESOURCE_H

#include "resource.h"
#include "path.h"

class PackageResource : public Resource {
public:
    PackageResource();

    ~PackageResource() override = default;

    void load() override;

    const void *const get() const;

private:
    Path path;
};

#endif //ACTIO_LIB_PACKAGE_RESOURCE_H
