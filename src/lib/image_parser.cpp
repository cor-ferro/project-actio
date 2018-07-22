#include "image_parser.h"
#include <fstream>
#include <IL/il.h>
#include <IL/ilu.h>
#include "../lib/console.h"

ImageData ImageParser::parse(char *data, size_t size) {
    parseByIl(data, size);
}

ImageData ImageParser::parseByIl(char *data, size_t size) {
    ImageData imageData;

    imageParserMutex.lock();

    ilInit();
    iluInit();

    ILuint id;
    ilGenImages(1, &id);
    ilBindImage(id);
    ILboolean isImageLoad = ilLoadL(IL_TYPE_UNKNOWN, reinterpret_cast<ILubyte*>(data), static_cast<ILuint>(size));
    if (!isImageLoad) {
        ILenum Error;
        while ((Error = ilGetError()) != IL_NO_ERROR) {
            std::string err;

            switch (Error) {
                case IL_NO_ERROR: err = "IL_NO_ERROR"; break;
                case IL_INVALID_ENUM: err = "IL_INVALID_ENUM"; break;
                case IL_OUT_OF_MEMORY: err = "IL_OUT_OF_MEMORY"; break;
                case IL_FORMAT_NOT_SUPPORTED: err = "IL_FORMAT_NOT_SUPPORTED"; break;
                case IL_INTERNAL_ERROR: err = "IL_INTERNAL_ERROR"; break;
                case IL_INVALID_VALUE: err = "IL_INVALID_VALUE"; break;
                case IL_ILLEGAL_OPERATION: err = "IL_ILLEGAL_OPERATION"; break;
                case IL_ILLEGAL_FILE_VALUE: err = "IL_ILLEGAL_FILE_VALUE"; break;
                case IL_INVALID_FILE_HEADER: err = "IL_INVALID_FILE_HEADER"; break;
                case IL_INVALID_PARAM: err = "IL_INVALID_PARAM"; break;
                case IL_COULD_NOT_OPEN_FILE: err = "IL_COULD_NOT_OPEN_FILE"; break;
                case IL_INVALID_EXTENSION: err = "IL_INVALID_EXTENSION"; break;
                case IL_FILE_ALREADY_EXISTS: err = "IL_FILE_ALREADY_EXISTS"; break;
                case IL_OUT_FORMAT_SAME: err = "IL_OUT_FORMAT_SAME"; break;
                case IL_STACK_OVERFLOW: err = "IL_STACK_OVERFLOW"; break;
                case IL_STACK_UNDERFLOW: err = "IL_STACK_UNDERFLOW"; break;
                case IL_INVALID_CONVERSION: err = "IL_INVALID_CONVERSION"; break;
                case IL_BAD_DIMENSIONS: err = "IL_BAD_DIMENSIONS"; break;
                case IL_FILE_READ_ERROR: err = "IL_FILE_READ/WRITE_ERROR"; break;
                default: err = "unknown";
            }

            console::warn("failed parse image %s", err);
        }

        imageParserMutex.unlock();

        return imageData;
    }

    imageData.initData(
            ilGetInteger(IL_IMAGE_WIDTH),
            ilGetInteger(IL_IMAGE_HEIGHT),
            ilGetInteger(IL_IMAGE_FORMAT)
    );

    imageData.set(ilGetData(), imageData.getWidth() * imageData.getHeight() * ImageData::componentSize(imageData.getFormat()));
    imageData.calcSize();

    ilBindImage(0);
    ilDeleteImage(id);

    imageParserMutex.unlock();


    return imageData;
}