#ifndef LIB_INILOADER_H_
#define LIB_INILOADER_H_

#include <unordered_map>
#include <glm/glm.hpp>
#include "iniparser/src/iniparser.h"
#include "iniparser/src/dictionary.h"
#include "path.h"
#include "utils.h"
#include "console.h"

struct IniLoader {
    struct Section {
        std::string name;
        std::string fullName;
        std::vector<std::string> fields;
        std::unordered_map<std::string, std::string> values;

        bool hasField(std::string &key) {
            auto it = values.find(key);

            return it != values.end();
        }

        bool hasNotEmpty(std::string &key) const {
            auto it = values.find(key);

            return it != values.end() && !it->second.empty();
        }

        template <class Vec>
        void getFieldVec(std::string key, Vec& value, Vec defaultValue = Vec(0.0f)) const {
            auto it = values.find(key);

            const_cast<Vec&>(value) = hasNotEmpty(key)
                ? utils::stovec<Vec>(it->second)
                : defaultValue;
        }

        void getField(std::string key, std::string& value, const std::string& defaultValue = "") const {
            auto it = values.find(key);
            value = hasNotEmpty(key) ? it->second : defaultValue;
        }

        void getField(std::string key, std::vector<std::string>& value, const char separator = ',') const {
            auto it = values.find(key);

            if (hasNotEmpty(key)) {
                value = utils::split(it->second, separator);
            }
        }

        void getField(std::string key, float& value) const {
            auto it = values.find(key);
            value = hasNotEmpty(key) ? std::stof(it->second) : 0.0f;
        }

        void getField(std::string key, int& value) const {
            auto it = values.find(key);
            value = hasNotEmpty(key) ? std::stoi(it->second) : 0;
        }

        void getField(std::string key, bool& value) const {
            auto it = values.find(key);
            if (!hasNotEmpty(key)) {
                value = false;
                return;
            }

            value = it->second == "true" ? true : false;
        }
    };

    ~IniLoader();

    void load(Path path);
    void defineSection(std::string name, std::vector<std::string> fields);
    const std::vector<Section>* getSections(const std::string& name);
    const Section* getSection(const std::string& name);

    private:
        dictionary * ini = nullptr;
        std::unordered_map<std::string, std::vector<std::string>> sectionDefines;
        std::unordered_map<std::string, std::vector<Section>> sections;
};

#endif