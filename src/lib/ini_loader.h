#ifndef LIB_INILOADER_H_
#define LIB_INILOADER_H_

#include <unordered_map>
#include <glm/glm.hpp>
#include "iniparser/src/iniparser.h"
#include "iniparser/src/dictionary.h"
#include "path.h"

struct IniLoader {
    struct Section {
        std::string name;
        std::vector<std::string> fields;
        std::unordered_map<std::string, std::string> values;

        bool hasField(std::string key) {
            auto it = values.find(key);

            return it != values.end();
        }

        std::string getFieldString(std::string key) {
            auto it = values.find(key);
            if (it == values.end()) return "";
            
            return it->second;
        }

        glm::vec3 getFieldVec(std::string key) {
            auto it = values.find(key);
            if (it == values.end()) return glm::vec3(0.0f);

            std::string str = it->second;
            const char delimiter = ',';
            std::string part = "";
            std::vector<std::string> parts;	

            for (auto it = str.begin(); it != str.end(); it++) {
                if ((*it) != delimiter) {
                    part+= (*it);
                } else {
                    parts.push_back(part);
                    part = "";
                }
            }

            if (part != "") {
                parts.push_back(part);
            }

            if (parts.size() == 1) {
                return glm::vec3(std::stof(parts[0]));
            } else if (parts.size() == 3) {
                
                return glm::vec3(
                    std::stof(parts[0]),
                    std::stof(parts[1]),
                    std::stof(parts[2])
                );
            } else {
                return glm::vec3(0.0f);
            }

            return glm::vec3(0.0f);
        }

        bool getFieldBool(std::string key) {
            auto it = values.find(key);
            if (it == values.end()) return false;

            return it->second == "true" ? true : false;
        }

        float getFieldFloat(std::string key) {
            auto it = values.find(key);
            if (it == values.end()) return 0.0f;

            return std::stof(it->second);
        }

        int getFieldInt(std::string key) {
            auto it = values.find(key);
            if (it == values.end()) return 0.0f;

            return std::stoi(it->second);
        }
    };

    ~IniLoader() {
        
    }
    
    void load(Path path) {
        dictionary * ini = iniparser_load(path.c_str());

        uint countSections = iniparser_getnsec(ini);

        console::infop("countSections: %i", countSections);

        std::vector<std::string> sectionNames;
        for (const auto&it : sectionDefines) {
            sectionNames.push_back(it.first);
        }

        for (uint i = 0; i < countSections; i++) {
            std::string sectionName = iniparser_getsecname(ini, i);

            for (const std::string& name : sectionNames) {
                std::string nameWithDot = name + ".";
                int len = nameWithDot.length();
                
                if (sectionName.compare(0, len, nameWithDot) == 0) {
                    auto it = sectionDefines.find(name);
                    if (it == sectionDefines.end()) {
                        continue;
                    }

                    std::vector<std::string>& sectionKeys = it->second;
                    
                    Section section;
                    section.name = sectionName;

                    for (const std::string& sectionKey : sectionKeys)
                    {
                        std::string key = sectionName + ":" + sectionKey;
                        std::string rawValue = iniparser_getstring(ini, key.c_str(), "");

                        section.values.insert({ sectionKey, rawValue });

                        console::infop("%s: %s", sectionKey.c_str(), rawValue.c_str());
                    }

                    auto sectionIt = sections.find(name);

                    if (sectionIt != sections.end()) {
                        sectionIt->second.push_back(section);
                    }
                }
            }
        }

        // iniparser_getsecname

        if (ini != nullptr) {
            iniparser_freedict(ini);
            ini = nullptr;
        }
    }

    void defineSection(std::string name, std::vector<std::string> fields) {
        sectionDefines.insert({ name, fields });
    }

    const std::vector<Section>* getSections(std::string name) {
        auto it = sections.find(name);

        if (it == sections.end()) {
            return nullptr;
        }

        return &it->second;
    }

    private:
        dictionary * ini = nullptr;
        std::unordered_map<std::string, std::vector<std::string>> sectionDefines;
        std::unordered_map<std::string, std::vector<Section>> sections;
};

#endif