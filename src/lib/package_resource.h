//
// Created by demitriy on 30.11.18.
//

#ifndef ACTIO_LIB_PACKAGE_RESOURCE_H
#define ACTIO_LIB_PACKAGE_RESOURCE_H

#include "resource.h"
#include "path.h"

class PackageResource : public Resource {
public:
    explicit PackageResource(const Path& path);

    ~PackageResource() override = default;

    void load() override;
};

#endif //ACTIO_LIB_PACKAGE_RESOURCE_H
