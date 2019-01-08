//
// Created by demitriy on 07.01.19.
//

#ifndef ACTIO_LOAD_STORY_COMPLETE_H
#define ACTIO_LOAD_STORY_COMPLETE_H

#include <string>

namespace game {
    namespace events {
        struct LoadStoryComplete {
            explicit LoadStoryComplete(const std::string& storyName, const std::string& chapterName)
                    : storyName(storyName)
                    , chapterName(chapterName)
            {}

            const std::string storyName;
            const std::string chapterName;
        };
    }
}

#endif //ACTIO_LOAD_STORY_COMPLETE_H
