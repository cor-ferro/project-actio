#include "shader_content.h"
#include <cstring>

namespace renderer {
    ShaderContent::ShaderContent() : content(std::make_shared<std::string>("")) {}

    bool ShaderContent::empty() const {
        return !content || content->empty();
    }

    std::shared_ptr<std::string> &ShaderContent::getContent() {
        return content;
    }
}