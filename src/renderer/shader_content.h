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

        bool empty() const;

        std::shared_ptr<std::string> &getContent();

    private:
        std::shared_ptr<std::string> content;
    };
}

#endif //ACTIO_RENDERER_SHADER_CONTENT_H
