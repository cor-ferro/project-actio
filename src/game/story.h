#ifndef ACTIO_GAME_STORY_H
#define ACTIO_GAME_STORY_H

#include <string>
#include <vector>
#include <map>
#include "chapter.h"
#include "assets.h"

namespace game {
    class Story;

    class StoryBuilder;

    class Story {
        friend StoryBuilder;

    private:
        typedef std::map<std::string, game::Chapter> StoryChapters;

    public:
        const std::string getName() {
            return name;
        }

        const StoryChapters getChapters() {
            return chapters;
        }

        Chapter *getChapter(const std::string &name) {
            auto it = chapters.find(name);

            if (it == chapters.end()) {
                return nullptr;
            }

            return &it->second;
        }

    private:
        std::string name;
        StoryChapters chapters;
    };


    class StoryBuilder {
    public:
        explicit StoryBuilder(Story &story) : story(story) {};

        void setName(const std::string &name) {
            story.name = name;
        }

        void addChapter(Chapter &chapter) {
            story.chapters[chapter.getName()] = chapter;
        }

    private:
        Story &story;
    };
}

#endif //ACTIO_GAME_STORY_H
