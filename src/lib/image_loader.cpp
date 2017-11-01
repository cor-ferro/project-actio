#include "./image_loader.h"

namespace Loader {
	ImageData * Image(char const * path) {
		return ImageIl(path);
	}

	inline ImageData * ImageSoil(char const * path) {
		ImageDataSoil * imageData = new ImageDataSoil();
		
		unsigned char * data = SOIL_load_image(path, &imageData->width, &imageData->height, 0, SOIL_LOAD_RGB);
		imageData->set(data, imageData->width * imageData->height);
		imageData->calcSize();

		return imageData;
	}

	inline ImageData * ImageIl(char const * path) {
		boost::mutex::scoped_lock lock(imageLoaderMutex);
		ilInit();
		iluInit();

		ImageDataIl * imageData = new ImageDataIl();

		ilGenImages(1, &imageData->id);
		ilBindImage(imageData->id);
		ILboolean isImageLoad = ilLoadImage(path);
		if (!isImageLoad) {
			ILenum Error;
			while ((Error = ilGetError()) != IL_NO_ERROR) {
				// printf("%d: %s/n", Error, iluErrorString());
				console::warn("error load image", path); 
			} 

			return imageData;
		}

		imageData->width = ilGetInteger(IL_IMAGE_WIDTH);
		imageData->height = ilGetInteger(IL_IMAGE_HEIGHT);
		imageData->format = ilGetInteger(IL_IMAGE_FORMAT);
		imageData->set(ilGetData(), imageData->width * imageData->height * ImageDataIl::componentSize(imageData->format));
		imageData->calcSize();
		ilBindImage(0);
		ilDeleteImage(imageData->id);

		return imageData;
	}

	ImageData::ImageData() 
		: width(0), 
		  height(0), 
		  data_(0), 
		  size(0) {}

	ImageData::~ImageData() {}

	void ImageData::calcSize() {
		size = width * height * sizeof(unsigned char);
	}

	bool ImageData::isReady() {
		return data_ != 0;
	}

	void ImageData::set(unsigned char * data, size_t size) {
		data_ = new unsigned char[size];
		for (size_t i = 0; i < size; i++) {
			data_[i] = data[i];
		}
	}

	unsigned char *  ImageData::get() {
		return data_;
	}

	void ImageData::free() {}


	ImageDataSoil::~ImageDataSoil() {}
	ImageDataIl::~ImageDataIl() {}

	void ImageDataSoil::free() {
		// SOIL_free_image_data(data_);
	}

	void ImageDataIl::free() {
		if (data_ != nullptr) {
			console::warn("free image");
			delete[] data_;
			data_ = nullptr;
		}
	}

	int ImageDataIl::componentSize(int format) {
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