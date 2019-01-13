#include "assets_model.h"
#include <assimp/include/assimp/postprocess.h>
#include "assets_loader.h"
#include "assets_texture.h"
#include "filesystem_resource.h"
#include "../core/geometry_builder.h"

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

        const auto *resource = dynamic_cast<const FilesystemResource*>(getResource());;

        const Path &path = resource->getPath();

        scene = importer.ReadFile(path.string().c_str(), flags);
    }

    const aiScene *Model::getScene() {
        if (!isLoaded()) {
            load();
            markLoaded();
        }

        return scene;
    }

    const std::vector <std::shared_ptr<::Mesh>> Model::createMeshes(MeshManager& meshManager) {
        std::vector <std::shared_ptr<::Mesh>> meshes;

        assert(loaded && "scene must be loaded");

        if (scene) {
            meshes.reserve(scene->mNumMeshes);

            for (uint i = 0; i < scene->mNumMeshes; i++) {
                const aiMesh* ai_mesh = scene->mMeshes[i];
                auto mesh = meshManager.create();

                GeometryBuilder::FromAi(mesh->geometry, ai_mesh);

                meshes.push_back(mesh);
            }
        }

        return meshes;
    }

    void Model::addMesh(std::shared_ptr<::Mesh> &mesh) {
        meshes.push_back(mesh);
    }

    const ::resources::Assimp *Model::getAiResource() {
        return assimpResource;
    }
}