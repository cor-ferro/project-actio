//
// Created by demitriy on 8/19/18.
//

#ifndef ACTIO_GAME_COMPONENTS_MESHES_H
#define ACTIO_GAME_COMPONENTS_MESHES_H

#include <vector>
#include <list>
#include <unordered_map>
#include <memory>
#include "../../core/mesh.h"

namespace game {
    namespace components {
        class Meshes {
        public:
            Meshes() = default;

            explicit Meshes(const std::vector<std::shared_ptr<Mesh>> &meshes) {
                this->meshes.insert(this->meshes.begin(), meshes.begin(), meshes.end());
            }

            explicit Meshes(const std::shared_ptr<Mesh> &mesh) {
                meshes.push_back(mesh);
            }

            const float height() {
                float maxHeight = 0.0f;

                for (auto &mesh : meshes) {
                    float geometryHeight = mesh->geometry.height();

                    if (geometryHeight > maxHeight) {
                        maxHeight = geometryHeight;
                    }
                }

                return maxHeight;
            };

            const float halfHeight() {
                float maxHeight = height();

                if (maxHeight > 0.0f) {
                    return maxHeight / 2.0f;
                }

                return 0.0f;
            };

            const Math::Box3 getBoundingBox() {
                Math::Box3 maxBox;

                for (auto &mesh : meshes) {
                    mesh->geometry.computeBoundingBox();
                    const Math::Box3 &bBox = mesh->geometry.getBoundingBox();

                    maxBox.radius.x = glm::max(bBox.radius.x, maxBox.radius.x);
                    maxBox.radius.y = glm::max(bBox.radius.y, maxBox.radius.y);
                    maxBox.radius.z = glm::max(bBox.radius.z, maxBox.radius.z);
                }

                return maxBox;
            };

            void reindexBones(std::unordered_map<std::string, uint> &nodeIndexes) {
                for (auto &mesh : meshes) {
                    for (unsigned int boneId = 0; boneId < mesh->bones.count; boneId++) {
                        auto nodeIndexIt = nodeIndexes.find(mesh->bones.names[boneId]);

                        if (nodeIndexIt != nodeIndexes.end()) {
                            mesh->bones.indexes[boneId] = nodeIndexIt->second;
                        } else {
                            console::warn("node not found: %s", mesh->bones.names[boneId]);
                            mesh->bones.indexes[boneId] = 0;
                        }
                    }
                }
            };

            std::vector<std::shared_ptr<Mesh>> &items() {
                return meshes;
            };

            std::shared_ptr<Mesh> &item() {
                return meshes[0];
            }
        private:
            std::vector<std::shared_ptr<Mesh>> meshes;
        };
    }
}

#endif //ACTIO_GAME_COMPONENTS_MESHES_H
