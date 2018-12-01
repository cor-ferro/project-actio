#ifndef ACTIO_GAME_STORY_IMPORTER_H
#define ACTIO_GAME_STORY_IMPORTER_H

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <streambuf>
#include "story.h"
#include "../lib/assets_loader.h"
#include "../resources/file_resource.h"
#include "../lib/data_loader.h"
#include "../lib/console.h"

namespace game {
    class StoryImporter {
    public:
        StoryImporter() = default;

        game::Story import(const resources::File &file);

        void loadScripts(const std::vector<std::string> &scripts, const boost::filesystem::path &fromPath);

        void loadTextures();

        void loadShaders();

        void loadModels();

        void loadMaterials();

        bool loadChapterAssets(Story &story, const std::string &chapterName, Assets *assets);

    private:
        boost::filesystem::path rootPath;
    };
}

#endif //ACTIO_GAME_STORY_IMPORTER_H
