#ifndef IMAGE_LOADER_H_
#define IMAGE_LOADER_H_

#include <boost/thread.hpp>
#include <GL/glew.h>
#include "SOIL.h"
#include <IL/il.h>
#include <IL/ilu.h>
#include "../lib/console.h"

namespace Loader {
	struct ImageData {
		ImageData();
		~ImageData();

		int width;
		int height;
		int size;
		int format;

		bool isReady();
		void calcSize();
		void set(unsigned char * data, size_t size);
		void free();
		unsigned char * get();
	protected:
		unsigned char * data_;
	};

	struct ImageDataSoil : ImageData {
		~ImageDataSoil();
		void free();
	};

	struct ImageDataIl : ImageData {
		~ImageDataIl();
		static int componentSize(int format);
		void free();

		ILuint id;
	};

	ImageData * Image(char const * path);
	inline ImageData * ImageSoil(char const * path);
	inline ImageData * ImageIl(char const * path);
}

static boost::mutex imageLoaderMutex;

#endif