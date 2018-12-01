#include "assets_model.h"
#include <assimp/include/assimp/postprocess.h>

namespace assets {

    /**
 * -------------------- Model --------------------
 */
    Model::Model(Resource *resource, const Options &options) : BaseAsset(resource), options(options) {}

    Model::~Model() {
        onDestroy();

        onLoad.disconnect_all_slots();
        onDestroy.disconnect_all_slots();

        if (assimpResource != nullptr) {
            delete assimpResource;
        }

        if (scene != nullptr) {
            importer.FreeScene();
        }
    }

    bool Model::hasOption(const std::string &key) const {
        return options.find(key) != options.end();
    }

    const std::string Model::getOption(const std::string &key) const {
        auto it = options.find(key);

        if (it != options.end()) {
            return it->second;
        }

        return "";
    }

    bool Model::isLoaded() {
        return loaded;
    }

    void Model::markLoaded() {
        loaded = true;
    }

    void Model::markUnLoaded() {
        loaded = false;
    }

    void Model::load() {
        bool flipUv = getOption("flipUv") == "true";

        unsigned int flags = aiProcessPreset_TargetRealtime_Quality
                             | aiProcess_GenSmoothNormals
                             | aiProcess_Triangulate
                             | aiProcess_CalcTangentSpace;

        if (flipUv) {
            flags |= aiProcess_FlipUVs;
        }

        const Resource *resource = getResource();
        const Path &path = resource->getPath();

        scene = importer.ReadFile(path.string().c_str(), flags);

        if (scene != nullptr) {
            assimpResource = new ::resources::Assimp(scene, path.string());
        }
    }

    const aiScene *Model::getScene() {
        if (!isLoaded()) {
            load();
            markLoaded();
        }

        return scene;
    }

    const std::vector <std::shared_ptr<::Mesh>> &Model::getMeshes() {
        return meshes;
    }

    void Model::addMesh(std::shared_ptr<::Mesh> &mesh) {
        meshes.push_back(mesh);
    }

    const ::resources::Assimp *Model::getAiResource() {
        return assimpResource;
    }
}