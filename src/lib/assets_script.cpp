#include "assets_script.h"

namespace assets {
    /**
     * -------------------- Script --------------------
     */
    Script::Script(assets::Resource *resource) : BaseAsset(resource) {
        content = nullptr;
    };

    void Script::setContent(Resource::Content newContent) {
        content = newContent;
    }

    Resource::Content Script::getContent() {
        if (!content) {
            setContent(resource->get());
        }

        return content;
    }
}