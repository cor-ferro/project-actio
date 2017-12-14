#ifndef IMAGE_LOADER_H_
#define IMAGE_LOADER_H_

#include <glad/glad.h>
#include <memory>
#include <unordered_map>
#include <boost/thread.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/regex.hpp>
#include "SOIL.h"
#include <IL/il.h>
#include <IL/ilu.h>
#include "../lib/console.h"

namespace ImageLoader {
	typedef char DataIndex;
	typedef unsigned char RawData;

	struct Data {
		Data();
		Data(const Data& other);
		Data(RawData * data, int width, int height);
		~Data();

		bool isReady();
		void calcSize();
		void set(RawData * data, size_t size);
		void free();
		RawData * get();

		int width;
		int height;
		int size;
		int format;
	protected:
		std::shared_ptr<RawData> data_;
	};

	Data load(std::string path);
	inline Data loadBySoil(std::string path);
	inline Data loadByIl(std::string path);

	int componentSize(int format);
}

static boost::mutex imageLoaderMutex;

#endif