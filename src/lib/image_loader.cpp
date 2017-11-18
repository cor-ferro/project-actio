#include "./image_loader.h"

namespace ImageLoader {
	Data load(std::string path) {
		// std::replace(path.begin(), path.end(), '\\', '/');
		
		// std::size_t fixPosE = path.find("e:");
		// std::size_t fixPosD = path.find("D:");
		// std::size_t fixPosWow = path.find("WoW");
		// if (fixPosE != std::string::npos) {
		// 	std::size_t endPos = path.find("shelves");
		// 	path.erase(fixPosE, endPos + 8 - fixPosE);
		// } else if (fixPosD != std::string::npos) {
		// 	std::size_t endPos = path.find("shelves");
		// 	path.erase(fixPosD, endPos + 8 - fixPosD);
		// } else if (fixPosWow != std::string::npos) {
		// 	std::size_t endPos = path.find(".fbm");
		// 	path.erase(fixPosWow - 16, endPos - fixPosWow + 16 + 4);
		// }


		return loadByIl(path);
	}

	inline Data loadBySoil(std::string path) {
		Data imageData;
		
		int chanels = 0;
		RawData * data = SOIL_load_image(path.c_str(), &imageData.width, &imageData.height, &chanels, SOIL_LOAD_AUTO);
		imageData.format = GL_RGB; // сделать автоопределение типа
		imageData.set(data, imageData.width * imageData.height * chanels);
		imageData.calcSize();
		SOIL_free_image_data(data);

		return imageData;
	}

	inline Data loadByIl(std::string path) {
		boost::mutex::scoped_lock lock(imageLoaderMutex);
		ilInit();
		iluInit();

		ILuint id;
		Data imageData;

		ilGenImages(1, &id);
		ilBindImage(id);
		ILboolean isImageLoad = ilLoadImage(path.c_str());
		if (!isImageLoad) {
			ILenum Error;
			while ((Error = ilGetError()) != IL_NO_ERROR) {
				// printf("%d: %s/n", Error, iluErrorString());
				console::warn("error load image", path.c_str()); 
			} 

			return imageData;
		}

		imageData.width = ilGetInteger(IL_IMAGE_WIDTH);
		imageData.height = ilGetInteger(IL_IMAGE_HEIGHT);
		imageData.format = ilGetInteger(IL_IMAGE_FORMAT);
		imageData.set(ilGetData(), imageData.width * imageData.height * componentSize(imageData.format));
		imageData.calcSize();
		ilBindImage(0);
		ilDeleteImage(id);

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
			case GL_LUMINANCE:
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
				console::warn("unknown image format", format);
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
	Data::Data(RawData * data, int width, int height) : width(width), height(height)
	{
		set(data, width * height);
		calcSize();
	}

	Data::~Data()
	{
		free();
	}

	RawData * Data::get()
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