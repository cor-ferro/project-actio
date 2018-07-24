#ifndef ACTIO_GAME_ASSETS_H
#define ACTIO_GAME_ASSETS_H

#include <vector>
#include <map>
#include <fstream>
#include <streambuf>
#include <cassert>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include "assets_resource.h"
#include "assets_types.h"
#include "../lib/console.h"
#include "../lib/data_loader.h"

namespace game {
    class Assets {
    public:
        typedef size_t Id;

        Assets() = default;

        ~Assets();

        std::map<Id, assets::Script> &getScripts();

        const std::map<Id, assets::Shader> &getShaders();

        const std::map<Id, assets::Model> &getModels();

        const std::map<Id, assets::Texture> &getTextures();

        const std::map<Id, assets::Material> &getMaterials();

        void addScript(assets::Resource *resource);

        void addTexture(assets::Resource *resource);

        void free();

    private:
        std::map<Id, assets::Shader> shaders;
        std::map<Id, assets::Texture> textures;
        std::map<Id, assets::Model> models;
        std::map<Id, assets::Script> scripts;
        std::map<Id, assets::Material> materials;

        Id idCounter = 0;

        Id genId();
    };
}

#endif //ACTIO_GAME_ASSETS_H
