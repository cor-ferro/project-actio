#include "text_set.h"

void TextSet::set(std::string key, std::string text) {
    data.insert({ key, text });
}

const std::string *TextSet::get(std::string key) const {
    const std::string *ptr = nullptr;
    auto it = data.find(key);

    if (it != data.end()) {
        ptr = &it->second;
    }

    return ptr;
}

const char *TextSet::getRaw(std::string key) const {
    const char *ptr = nullptr;
    auto it = data.find(key);

    if (it != data.end()) {
        ptr = it->second.c_str();
    }

    return ptr;
}