//
// Created by demitriy on 7/22/18.
//

#ifndef ACTIO_TEXT_SET_H
#define ACTIO_TEXT_SET_H

#include <unordered_map>

class TextSet {
public:
    void set(std::string key, std::string text);

    const std::string *get(std::string key) const;

    const char *getRaw(std::string key) const;

private:
    std::unordered_map<std::string, std::string> data;
};

#endif //ACTIO_TEXT_SET_H
