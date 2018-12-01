//
// Created by demitriy on 9/30/18.
//

#ifndef ACTIO_ASSETS_MODEL_H
#define ACTIO_ASSETS_MODEL_H

#include <unordered_map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <boost/signals2.hpp>
#include "assets_base.h"
#include "resource.h"
#include "../resources/assimp_resource.h"
#include "../core/model.h"
#include "../core/skin.h"
#include "../core/mesh.h"

namespace assets {
    class Model : public BaseAsset {
    public:
        typedef std::unordered_map<std::string, std::string> Options;

        explicit Model(Resource *resource, const Options &options);

        ~Model();

        void load();

        bool hasOption(const std::string &key) const;

        const std::string getOption(const std::string &key) const;

        bool isLoaded();

        void markLoaded();

        void markUnLoaded();

        const aiScene *getScene();

        const ::resources::Assimp *getAiResource();

        void addMesh(std::shared_ptr<::Mesh> &mesh);

        const std::vector<std::shared_ptr<::Mesh>> &getMeshes();

        boost::signals2::signal<void ()> onLoad;
        boost::signals2::signal<void ()> onDestroy;
    private:
        Assimp::Importer importer;
        const aiScene *scene = nullptr;
        ::resources::Assimp *assimpResource = nullptr;

        bool loaded = false;
        Options options;

        std::vector<std::shared_ptr<Material>> materials;
        std::vector<std::shared_ptr<::Mesh>> meshes;
    };
}

#endif //ACTIO_ASSETS_MODEL_H
