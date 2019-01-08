//
// Created by demitriy on 07.01.19.
//

#ifndef ACTIO_LOAD_STORY_H
#define ACTIO_LOAD_STORY_H

#include <string>

namespace game {
    namespace events {
        struct LoadStory {
            explicit LoadStory(const std::string& storyName, const std::string& chapterName)
                : storyName(storyName)
                , chapterName(chapterName)
                {}

            const std::string storyName;
            const std::string chapterName;
        };
    }
}

#endif //ACTIO_LOAD_STORY_H
