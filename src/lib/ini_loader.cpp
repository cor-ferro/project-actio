#include "ini_loader.h"

IniLoader::~IniLoader() {}

void IniLoader::load(Path path) {
    dictionary * ini = iniparser_load(path.c_str());

    uint countSections = iniparser_getnsec(ini);

    console::info("countSections: %i", countSections);

    std::vector<std::string> sectionNames;
    for (const auto&it : sectionDefines) {
        sectionNames.push_back(it.first);
    }

    for (uint i = 0; i < countSections; i++) {
        std::string sectionName = iniparser_getsecname(ini, i);

        for (const std::string& name : sectionNames) {
            auto it = sectionDefines.find(name);
            if (it == sectionDefines.end()) {
                continue;
            }

            std::string nameWithDot = name + ".";
            unsigned long int lenWithDot = nameWithDot.length();
            unsigned long int len = name.length();

            bool isMultipleSection = sectionName.compare(0, lenWithDot, nameWithDot) == 0;
            bool isSingleSection = sectionName.compare(0, sectionName.length(), name) == 0;
            
            if (isSingleSection || isMultipleSection) {
                std::vector<std::string>& sectionKeys = it->second;
                
                Section section;

                if (isSingleSection) {
                    section.name = sectionName;
                    section.fullName = sectionName;
                } else if (isMultipleSection) {
                    std::size_t pos = sectionName.find(".");
                    std::string nameAfterDot = sectionName.substr(pos + 1);

                    section.name = nameAfterDot;
                    section.fullName = sectionName;
                }

                for (const std::string& sectionKey : sectionKeys)
                {
                    std::string key = sectionName + ":" + sectionKey;
                    std::string rawValue = iniparser_getstring(ini, key.c_str(), "");

                    section.values.insert({ sectionKey, rawValue });
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

void IniLoader::defineSection(std::string name, std::vector<std::string> fields) {
    sectionDefines.insert({ name, fields });
    sections.insert({ name, {} });
}

const std::vector<IniLoader::Section>* IniLoader::getSections(const std::string& name) {
    auto it = sections.find(name);

    if (it == sections.end()) {
        return nullptr;
    }

    return &it->second;
}

const IniLoader::Section* IniLoader::getSection(const std::string& name) {
    auto it = sections.find(name);

    if (it == sections.end()) {
        console::info("empty 1");
        return nullptr;
    }

    if (it->second.size() == 0) {
        console::info("empty 2");
        return nullptr;
    }

    return &it->second[0];
}