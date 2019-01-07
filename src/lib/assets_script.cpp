#include "assets_script.h"

namespace assets {
    /**
     * -------------------- Script --------------------
     */
    Script::Script(Resource *resource) : BaseAsset(resource) {};

    void Script::setContent(const std::string& newContent) {
        content = newContent;
    }

    const std::string& Script::getContent() {
        if (content.empty()) {
            const void *data = resource->get();

            content = std::string(static_cast<const char *>(data), resource->getSize()); // Каков размер? Есть ли завершающий байт

        }

        return content;
    }
}