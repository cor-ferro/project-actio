//
// Created by demitriy on 7/22/18.
//

#ifndef ACTIO_IMAGE_PARSER_H
#define ACTIO_IMAGE_PARSER_H

#include <boost_1_65_1/boost/thread/mutex.hpp>
#include "image_data.h"

static boost::mutex imageParserMutex;

class ImageParser {
public:
    ImageData parse(char *data, size_t size);

private:
    ImageData parseByIl(char *data, size_t size);
};

#endif //ACTIO_IMAGE_PARSER_H
