#ifndef ACTIO_LIB_RESOURCE_H
#define ACTIO_LIB_RESOURCE_H

#include <string>
#include "path.h"


class Resource {
public:
    typedef std::shared_ptr<std::string> Content;

    explicit Resource(Path path);

    virtual ~Resource() = default;

    void reset();

    Content get();

    const size_t getSize();

    virtual void load() = 0;

    bool isLoaded();

    const Path &getPath() const;

protected:
    Path path;
    bool loaded = false;
    std::shared_ptr<std::string> data;
    size_t size = 0;
};

#endif //ACTIO_LIB_RESOURCE_H
