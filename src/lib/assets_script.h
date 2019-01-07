//
// Created by demitriy on 9/30/18.
//

#ifndef ACTIO_ASSETS_SCRIPT_H
#define ACTIO_ASSETS_SCRIPT_H

#include "assets_base.h"
#include "resource.h"
#include <string>

namespace assets {
    class Script : public BaseAsset {
    public:
        explicit Script(Resource *resource);

        void setContent(const std::string& newContent);

        const std::string& getContent();

    private:
        std::string content;
    };
}

#endif //ACTIO_ASSETS_SCRIPT_H
