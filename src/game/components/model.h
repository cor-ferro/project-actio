//
// Created by demitriy on 3/24/18.
//

#ifndef ACTIO_MODEL_H
#define ACTIO_MODEL_H

#include <memory>
#include <unordered_map>
#include <map>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <ozz/base/memory/allocator.h>
#include <ozz/animation/runtime/skeleton.h>
#include <ozz/animation/offline/raw_skeleton.h>
#include <ozz/animation/offline/skeleton_builder.h>
#include "../../core/mesh.h"
#include "../../lib/image_loader.h"
#include "../../lib/comparators.h"
#include "../../resources/resources.h"
#include "../../animation/animation.h"
#include "../../animation/process.h"

namespace game {
    namespace components {
        typedef std::string ModelName;
        typedef std::vector<Mesh*> ModelMeshes;

        struct Model {
            struct Config {
                Config();
                Config(const Config& other);

                std::string name;
                Resource::File file;
                bool flipUv;
                vec3 position;
                vec3 rotation;
                float rotationAngle;
                vec3 scale;
                std::string animation;
            };

            struct Node {
                Node();
                Node(aiNode * node);
                ~Node();

                void addMesh(Mesh * mesh);
                void addNode(Node * node);

                std::string name;
                std::vector<Node*> children;
                std::vector<Mesh*> meshes;
                mat4 transformation;
            };

            struct BoneMap {
                uint jointIndex;
                uint boneIndex;
                mat4 offset;
            };

            Model();
//            Model(Config& modelConfig);
            Model(Mesh * mesh);
            ~Model();

            const ModelName& getName();

            void setName(std::string newName);

            void initFromAi(const Resource::Assimp * assimpResource);
            void addMesh(Mesh * mesh);
            void addNode(Node * node);

            void removeMesh(Mesh * mesh);
            void removeNode(Node * node);

            const ModelMeshes& getMeshes();
            Mesh * getFirstMesh();

            const int getNodesCount();
        private:
            void destroy();

            void freeMeshes();
            void freeNodes();

            ModelName name_;
            ModelMeshes meshes_;
            Node * rootNode_ = nullptr;
            std::unordered_map<std::string, std::shared_ptr<const Node>> nodes_;
            std::unordered_map<std::string, ImageLoader::Data> images_;
        };
    }
}

#endif //ACTIO_MODEL_H