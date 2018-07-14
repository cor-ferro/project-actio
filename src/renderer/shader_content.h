//
// Created by demitriy on 7/8/18.
//

#ifndef ACTIO_RENDERER_SHADER_CONTENT_H
#define ACTIO_RENDERER_SHADER_CONTENT_H

#include <memory>
#include <cstring>
#include <string>

namespace renderer {
    struct ShaderContent {
        ShaderContent();

        explicit ShaderContent(std::string);

        ShaderContent(const ShaderContent &other);

        bool empty() const ;

        void set(size_t size, const char *newContent);

        void set(const std::string &newContent);

        const char *const get();

    private:
        std::shared_ptr<char> content;
    };
}

#endif //ACTIO_RENDERER_SHADER_CONTENT_H
