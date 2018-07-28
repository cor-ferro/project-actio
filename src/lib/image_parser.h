#ifndef ACTIO_IMAGE_PARSER_H
#define ACTIO_IMAGE_PARSER_H

#include <boost_1_65_1/boost/thread/mutex.hpp>
#include "image_data.h"

static boost::mutex imageParserMutex;

class ImageParser {
public:
    std::shared_ptr<ImageData> parse(const char *data, size_t size);

private:
    std::shared_ptr<ImageData> parseByIl(const char *data, size_t size);
};

#endif //ACTIO_IMAGE_PARSER_H
