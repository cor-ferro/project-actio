#include <queue>
#include <thread>
#include <mutex>
#include "assimp.h"
#include "image_data.h"
#include "image_parser.h"
#include "utils.h"
#include "assets_loader.h"
#include "../core/texture.h"
#include "../core/material.h"

namespace libAi {
    mat4 toNativeType(const aiMatrix4x4 &aiMat) {
        const float values[16] = {
                aiMat.a1, aiMat.b1, aiMat.c1, aiMat.d1,
                aiMat.a2, aiMat.b2, aiMat.c2, aiMat.d2,
                aiMat.a3, aiMat.b3, aiMat.c3, aiMat.d3,
                aiMat.a4, aiMat.b4, aiMat.c4, aiMat.d4
        };

        return glm::make_mat4<float>(values);
    }

    vec3 toNativeType(const aiVector3D &aiVec) {
        return vec3(aiVec.x, aiVec.y, aiVec.z);
    }

    quat toNativeType(const aiQuaternion &aiQuat) {
        return quat(aiQuat.w, aiQuat.x, aiQuat.y, aiQuat.z);
    }

    void print(const aiMatrix4x4 &aiMat) {
        using namespace std;

        auto fmt = [](float number) { return to_string(number).length(); };

        int aColMax = std::max({fmt(aiMat.a1), fmt(aiMat.a2), fmt(aiMat.a3), fmt(aiMat.a4)});
        int bColMax = std::max({fmt(aiMat.b1), fmt(aiMat.b2), fmt(aiMat.b3), fmt(aiMat.b4)});
        int cColMax = std::max({fmt(aiMat.c1), fmt(aiMat.c2), fmt(aiMat.c3), fmt(aiMat.c4)});
        int dColMax = std::max({fmt(aiMat.d1), fmt(aiMat.d2), fmt(aiMat.d3), fmt(aiMat.d4)});

        const int indent = 2;

        console::info("mat:");
        std::cout << "[";
        std::cout << std::setw(aColMax) << aiMat.a1;
        std::cout << std::setw(bColMax + indent) << aiMat.b1;
        std::cout << std::setw(cColMax + indent) << aiMat.c1;
        std::cout << std::setw(dColMax + indent) << aiMat.d1;
        std::cout << "]";
        std::cout << std::endl;
        std::cout << "[";
        std::cout << std::setw(aColMax) << aiMat.a2;
        std::cout << std::setw(bColMax + indent) << aiMat.b2;
        std::cout << std::setw(cColMax + indent) << aiMat.c2;
        std::cout << std::setw(dColMax + indent) << aiMat.d2;
        std::cout << "]";
        std::cout << std::endl;
        std::cout << "[";
        std::cout << std::setw(aColMax) << aiMat.a3;
        std::cout << std::setw(bColMax + indent) << aiMat.b3;
        std::cout << std::setw(cColMax + indent) << aiMat.c3;
        std::cout << std::setw(dColMax + indent) << aiMat.d3;
        std::cout << "]";
        std::cout << std::endl;
        std::cout << "[";
        std::cout << std::setw(aColMax) << aiMat.a4;
        std::cout << std::setw(bColMax + indent) << aiMat.b4;
        std::cout << std::setw(cColMax + indent) << aiMat.c4;
        std::cout << std::setw(dColMax + indent) << aiMat.d4;
        std::cout << "]";
        std::cout << std::endl;
    }

    void print(const aiVector3D &aiVec) {
        console::info("vec: %.2f %.2f %.2f", aiVec.x, aiVec.y, aiVec.z);
    }

    void print(const aiQuaternion &aiQuat) {
        console::info("quat: %.2f %.2f %.2f %.2f", aiQuat.x, aiQuat.y, aiQuat.z, aiQuat.w);
    }

    Material createMaterial(const aiMaterial *assimpMaterial, const Resource::Assimp *assimpResource, Assets *assets) {
        Material material;

        if (!assimpMaterial) {
            material.initEmptyTextures();
            return std::move(material);
        }

        Texture diffuseMap = Texture::Empty(Texture::Type::Diffuse, 0);
        Texture specularMap = Texture::Empty(Texture::Type::Specular, 255);
        Texture heightMap = Texture::Empty(Texture::Type::Height, 0);
        Texture normalMap = Texture::Empty(Texture::Type::Normal, 0);

        float shininessValue = 0.0f;
        bool isWireframe = false;

        aiColor3D diffuseColor(0.f, 0.f, 0.f);
        aiColor3D specularColor(0.f, 0.f, 0.f);
        aiColor3D ambientColor(0.f, 0.f, 0.f);
        assimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
        assimpMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
        assimpMaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor);

        assimpMaterial->Get(AI_MATKEY_SHININESS, shininessValue);
        assimpMaterial->Get(AI_MATKEY_ENABLE_WIREFRAME, isWireframe);

        unsigned int countDiffuseTextures = assimpMaterial->GetTextureCount(aiTextureType_DIFFUSE);
        unsigned int countSpecularTextures = assimpMaterial->GetTextureCount(aiTextureType_SPECULAR);
        unsigned int countHeightTextures = assimpMaterial->GetTextureCount(aiTextureType_HEIGHT);
        unsigned int countNormalTextures = assimpMaterial->GetTextureCount(aiTextureType_NORMALS);

        assets::Loader assetsLoader(assimpResource->getBasePath().string());

        std::vector<std::pair<std::string, Texture*>> texturesToLoad;

        if (countDiffuseTextures > 0) {
            aiString tex;
            assimpMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &tex);

            std::string texturePath(tex.C_Str());
            texturesToLoad.push_back({ texturePath, &diffuseMap });
            diffuseColor = aiColor3D(0.f, 0.f, 0.f);
            ambientColor = aiColor3D(0.f, 0.f, 0.f);
        }

        if (countSpecularTextures > 0) {
            aiString tex;
            assimpMaterial->GetTexture(aiTextureType_SPECULAR, 0, &tex);

            std::string texturePath(tex.C_Str());
            texturesToLoad.push_back({ texturePath, &specularMap });
            specularColor = aiColor3D(0.f, 0.f, 0.f);
        }

        if (countHeightTextures > 0) {
            aiString tex;
            assimpMaterial->GetTexture(aiTextureType_HEIGHT, 0, &tex);

            std::string texturePath(tex.C_Str());
            texturesToLoad.push_back({ texturePath, &heightMap });
        }

        if (countNormalTextures > 0) {
            aiString tex;
            assimpMaterial->GetTexture(aiTextureType_NORMALS, 0, &tex);

            std::string texturePath(tex.C_Str());
            texturesToLoad.push_back({ texturePath, &normalMap });
        }

        for (auto &it : texturesToLoad) {
            std::string path = it.first;
            Texture *textureMap = it.second;

            assets::Resource *resource = assetsLoader.load(path);
            assets::Texture *assetTexture = nullptr;
            std::shared_ptr<ImageData> image;

            if (resource != nullptr) {
                assetTexture = assets->addTexture(resource);
            } else {
                assetTexture = assets->getTexture("DefaultTexture");
            }

            if (assetTexture != nullptr) {
                image = assetTexture->getImage();
                textureMap->setData(image);
            }
        }

        material.setDiffuse(diffuseColor[0], diffuseColor[1], diffuseColor[2]);
        material.setSpecular(specularColor[0], specularColor[1], specularColor[2]);
        material.setAmbient(ambientColor[0], ambientColor[1], ambientColor[2]);
        material.setWireframe(isWireframe);
        material.setShininess(shininessValue);

        material.setDiffuseMap(diffuseMap);
        material.setSpecularMap(specularMap);
        material.setNormalMap(normalMap);
        material.setHeightMap(heightMap);

        return std::move(material);
    }
}
