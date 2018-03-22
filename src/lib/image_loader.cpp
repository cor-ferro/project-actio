#include "./image_loader.h"

namespace ImageLoader {
	Data load(std::string path) {
		std::replace(path.begin(), path.end(), '\\', '/');

		return loadByIl(path);
	}

	inline Data loadBySoil(std::string path) {
		Data imageData;
		
		int chanels = 0;
		RawData * data = SOIL_load_image(path.c_str(), &imageData.width, &imageData.height, &chanels, SOIL_LOAD_RGB);
		imageData.format = GL_RGB; // сделать автоопределение типа
		imageData.set(data, imageData.width * imageData.height * chanels);
		imageData.calcSize();
		SOIL_free_image_data(data);

		return imageData;
	}

	inline Data loadByIl(std::string path) {
		std::ifstream file(path.c_str(), std::ios::in|std::ios::binary|std::ios::ate);
		Data imageData;

		if (file.is_open()) {
			std::streampos size;
			size = file.tellg();
			char * memblock = new char[size];
			file.seekg(0, std::ios::beg);
			file.read(memblock, size);
			file.close();

			imageLoaderMutex.lock();

			ilInit();
			iluInit();

			ILuint id;
			ilGenImages(1, &id);
			ilBindImage(id);
			ILboolean isImageLoad = ilLoadL(IL_TYPE_UNKNOWN, (ILubyte*)memblock, size);
			if (!isImageLoad) {
				ILenum Error;
				while ((Error = ilGetError()) != IL_NO_ERROR) {
					std::string err("unknown");
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
					}

					console::warn("error load image %s, %s", path, err);
				}

				imageLoaderMutex.unlock();
				
				return imageData;
			}

			imageData.width = ilGetInteger(IL_IMAGE_WIDTH);
			imageData.height = ilGetInteger(IL_IMAGE_HEIGHT);
			imageData.format = ilGetInteger(IL_IMAGE_FORMAT);

			imageData.set(ilGetData(), imageData.width * imageData.height * componentSize(imageData.format));
			imageData.calcSize();

			ilBindImage(0);
			ilDeleteImage(id);

			imageLoaderMutex.unlock();

			delete[] memblock;
		} else {
			console::warn("no such file or directory: %s", path);
		}

		return imageData;
	}

	int componentSize(int format)
	{
		switch(format) {
			case GL_RG:
				return 2;
			
			case GL_RED:
			case GL_RGB:
			case GL_BGR:
				return 3;
			
			case GL_RGBA:
			case GL_BGRA:
			case 6409: // GL_LUMINANCE
				return 4;

			case GL_RG_INTEGER:
				return 2;

			case GL_RED_INTEGER:
			case GL_RGB_INTEGER:
			case GL_BGR_INTEGER:
				return 3;

			case GL_RGBA_INTEGER:
			case GL_BGRA_INTEGER:
				return 4;
			default:
				console::warn("unknown image format %s", format);
				return 3;
		}
	}
}

namespace ImageLoader {
	Data::Data(const Data& other) {
		width = other.width;
		height = other.height;
		size = other.size;
		format = other.format;
		data_ = other.data_;
	}

	Data::Data() : width(0), height(0), data_(0), size(0) {}
	Data::Data(RawData * data, int width, int height, int format)
		: width(width)
		, height(height)
		, format(format)
	{
		set(data, width * height * componentSize(format));
		calcSize();
	}

	Data::~Data()
	{
		free();
	}

	RawData * Data::get() const
	{
		return data_.get();
	}

	void Data::calcSize()
	{
		size = width * height * sizeof(RawData);
	}

	bool Data::isReady()
	{
		return data_ != nullptr;
	}

	void Data::set(RawData * data, size_t size)
	{
		// data_ = data;
		RawData * newData = new RawData[size];
		for (size_t i = 0; i < size; i++) {
			newData[i] = data[i];
		}

		data_.reset(newData);
	}

	void Data::free()
	{
		// console::warn("free image");
	}
}
