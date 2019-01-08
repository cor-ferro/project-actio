#include "utils.h"
#include "console.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

namespace utils {
    std::string formatMemorySize(std::size_t size) {
        std::stringstream ss;

        if (size >= 1073741824) {
            ss << (size / 1024 / 1024) << " " << "Gb";
        } else if (size >= 1048576) {
            ss << (size / 1024 / 1024) << " " << "Mb";
        } else if (size > 1000) {
            ss << (size / 1024) << " " << "Kb";
        } else {
            ss << (size) << " " << "byte";
        }

        std::string finalStr = ss.str();

        return finalStr;
    }

    const std::vector<std::string> parseVec(const std::string &strVec) {
        const char delimiter = ',';
        std::string part = "";
        std::vector<std::string> parts;

        for (auto it = strVec.begin(); it != strVec.end(); it++) {
            if ((*it) != delimiter) {
                part += (*it);
            } else {
                parts.push_back(part);
                part = "";
            }
        }

        if (part != "") {
            parts.push_back(part);
        }

        return parts;
    }

    template<>
    const vec2 stovec<vec2>(const std::string &strVec) {
        const std::vector<std::string> parts = parseVec(strVec);

        switch (parts.size()) {
            case 4:
                return vec2(std::stof(parts[0]), std::stof(parts[1]));
            case 1:
                return vec2(std::stof(parts[0]));
            default:
                return vec2(0.0f);
        }
    }

    template<>
    const vec3 stovec<vec3>(const std::string &strVec) {
        const std::vector<std::string> parts = parseVec(strVec);

        switch (parts.size()) {
            case 3:
                return vec3(std::stof(parts[0]), std::stof(parts[1]), std::stof(parts[2]));
            case 1:
                return vec3(std::stof(parts[0]));
            default:
                return vec3(0.0f);
        }
    }

    template<>
    const vec4 stovec<vec4>(const std::string &strVec) {
        const std::vector<std::string> parts = parseVec(strVec);

        switch (parts.size()) {
            case 4:
                return vec4(std::stof(parts[0]), std::stof(parts[1]), std::stof(parts[2]), std::stof(parts[3]));
            case 1:
                return vec4(std::stof(parts[0]));
            default:
                return vec4(0.0f);
        }
    }

    std::vector<std::string> split(std::string str, const char separator) {
        std::vector<std::string> items;
        std::istringstream stream(str);
        std::string token;

        while (std::getline(stream, token, separator)) {
            items.push_back(token);
        }

        return items;
    }

    Data getFileContents(const Path& path) {
        std::ifstream stream(path.string(), std::ios::in | std::ios::binary);

        if (boost::filesystem::exists(path)) {
            size_t fileSize = boost::filesystem::file_size(path);
            return Data{stream, fileSize};
        }

        return Data{};
    }

    void getFileContents(const Path &path, Data& data) {
        std::ifstream stream(path.string(), std::ios::in | std::ios::binary);

        if (stream) {
            size_t fileSize = boost::filesystem::file_size(path);
            data.set(stream, fileSize);
        }
    }

    float scaleNumber(float x, float y, float value) {
        float v = glm::clamp(value, x, y);
        float dist = glm::abs(y) - glm::abs(x);

        return (v - x) / dist;
    }
}
