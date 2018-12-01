//
// Created by demitriy on 11/10/18.
//

#ifndef ACTIO_MESH_MANAGER_H
#define ACTIO_MESH_MANAGER_H

#include <memory>
#include <functional>
#include <boost/pool/object_pool.hpp>
#include "../memory/poolallocator.h"
#include "../core/mesh.h"

class MeshManager {
public:
    MeshManager();

    ~MeshManager();

    inline std::shared_ptr<Mesh> create() {
        Mesh *mesh = pool.construct();

        mesh->material.reset(new Material());

        std::shared_ptr<Mesh> ptr(mesh, [this](Mesh *mesh) { this->pool.destroy(mesh); });

        return ptr;
    }

    template <typename ...Arg>
    inline std::shared_ptr<Mesh> create(Arg... arg) {
        Mesh *mesh = pool.construct(arg...);

        std::shared_ptr<Mesh> ptr(mesh, [this](Mesh *mesh) { this->pool.destroy(mesh); });

        return ptr;
    }

    void init();

    void destroy();

private:
    boost::object_pool<Mesh> pool;
};

#endif //ACTIO_MESH_MANAGER_H
