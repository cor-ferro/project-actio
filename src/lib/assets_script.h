//
// Created by demitriy on 9/30/18.
//

#ifndef ACTIO_ASSETS_SCRIPT_H
#define ACTIO_ASSETS_SCRIPT_H

#include "assets_base.h"
#include "assets_resource.h"

namespace assets {
    class Script : public BaseAsset {
    public:
        explicit Script(assets::Resource *resource);

        void setContent(Resource::Content newContent);

        Resource::Content getContent();

    private:
        Resource::Content content;
    };
}

#endif //ACTIO_ASSETS_SCRIPT_H
