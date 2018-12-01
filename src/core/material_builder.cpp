#include "material_builder.h"
#include <assimp/scene.h>
#include "../lib/assets_loader.h"

void MaterialBuilder::FromAi(std::shared_ptr<Material> &material, const aiMaterial *assimpMaterial, const resources::Assimp *assimpResource, Assets *assets) {
    if (!assimpMaterial) {
        material->initEmptyTextures();
        return;
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

        Resource *resource = assetsLoader.createResource(path);
        assets::Texture *assetTexture = nullptr;
        std::shared_ptr<ImageData> image;

        if (resource != nullptr) {
            assetTexture = assets->addTexture(resource);
        } else {
            assetTexture = assets->getDefaultTexture(*textureMap);
        }

        if (assetTexture != nullptr) {
            image = assetTexture->getImage();
            textureMap->setData(image);
        }
    }

    material->setDiffuse(diffuseColor[0], diffuseColor[1], diffuseColor[2]);
    material->setSpecular(specularColor[0], specularColor[1], specularColor[2]);
    material->setAmbient(ambientColor[0], ambientColor[1], ambientColor[2]);
    material->setWireframe(isWireframe);
    material->setShininess(shininessValue);

    material->setDiffuseMap(diffuseMap);
    material->setSpecularMap(specularMap);
    material->setNormalMap(normalMap);
    material->setHeightMap(heightMap);
}