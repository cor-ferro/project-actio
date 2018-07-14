#include "shader_content.h"
#include <cstring>

namespace renderer {
    ShaderContent::ShaderContent() : content(nullptr) {}

    ShaderContent::ShaderContent(std::string newContent) {
        set(newContent);
    }

    ShaderContent::ShaderContent(const ShaderContent &other) {
        content = other.content;
    }

    bool ShaderContent::empty() const {
        return content == nullptr;
    }

    void ShaderContent::set(size_t size, const char *newContent) {
        content.reset(new char[size]);
        strcpy(content.get(), newContent);
    }

    void ShaderContent::set(const std::string &newContent) {
        content.reset(new char[newContent.length()]);
        strcpy(content.get(), newContent.c_str());
    }

    const char *const ShaderContent::get() {
        return content.get();
    }
}