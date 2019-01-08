#ifndef ACTIO_IMAGE_PARSER_H
#define ACTIO_IMAGE_PARSER_H

#include <boost_1_65_1/boost/thread/mutex.hpp>
#include "image_data.h"

static boost::mutex imageParserMutex;

class ImageParser {
public:
    enum class Type {
        IL,
        SOIL
    };

    std::shared_ptr<ImageData> parse(const unsigned char *data, const size_t &size);

    std::shared_ptr<ImageData> parse(const unsigned char *data, const size_t &size, const ImageParser::Type &type);

private:
    std::shared_ptr<ImageData> parseByIl(const unsigned char *data, const size_t &size);

    std::shared_ptr<ImageData> parseBySoil(const unsigned char *data, const size_t &size);
};

#endif //ACTIO_IMAGE_PARSER_H
