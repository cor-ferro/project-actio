#ifndef U_BUFFER_H_
#define U_BUFFER_H_

#include <glad/glad.h>
#include <cstddef>
#include <vector>
#include <unordered_map>
#include <utility>
#include "../../lib/console.h"

#define UBUFFER_DEFAULT_GROUP_NAME "main"

namespace renderer {
	namespace Opengl {
		struct UBufferData {
			UBufferData();
			UBufferData(std::vector<std::size_t> sizes);
			UBufferData(uint count, std::vector<std::size_t> sizes);
			UBufferData(const UBufferData& other);

			const std::vector<std::size_t>& getSizes() const;
			const uint getCount() const;
		private:
			uint count;
			std::vector<std::size_t> sizes;
		};

		struct UBUfferOffset {
			UBUfferOffset();
			UBUfferOffset(const UBufferData& data, size_t& globalOffset);
			UBUfferOffset(const UBUfferOffset& other);

			const std::pair<std::size_t, std::size_t>& get(uint index) const;
			const std::pair<std::size_t, std::size_t>& get(uint offsetIndex, uint index) const;
		private:
			uint count;
			std::vector<std::pair<std::size_t, std::size_t>> offsets;
		};

		typedef std::unordered_map<std::string, UBufferData> UBufferMapSizes;

		struct UBuffer {
			UBuffer();
			~UBuffer();

			void init(std::vector<std::size_t> sizes, GLuint mountPoint);
			void init(UBufferMapSizes mapSizes, GLuint mountPoint);

			void set(short int index, const GLvoid * data);
			void set(std::string group, short int index, const GLvoid * data);
			void set(std::string group, short int offsetIndex, short int index, const GLvoid * data);
			void use();
			void nouse();

			void setDataSize(size_t size);
			const size_t& getDataSize();

		private:
			GLuint uboHandle_;
			size_t dataSize_;
			bool isUsed_;
			std::unordered_map<std::string, UBUfferOffset> offsets_;
		};
	}
}


#endif /* U_BUFFER_H_ */
