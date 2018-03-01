#ifndef SRC_MEMORY_UTILS_H_
#define SRC_MEMORY_UTILS_H_

namespace memory {
	class Utils {
	public:
		static const std::size_t kbytes(unsigned int count) {
			return count * 1024;
		}

		static const std::size_t mbytes(unsigned int count) {
			return count * 1024 * 1024;
		}

		static const std::size_t gbytes(unsigned int count) {
			return count * 1024 * 1024 * 1024;
		}

		inline static const std::size_t kb(unsigned int count) {
			return kbytes(count);
		}

		inline static const std::size_t mb(unsigned int count) {
			return mbytes(count);
		}

		inline static const std::size_t gb(unsigned int count) {
			return gbytes(count);
		}

		static const std::size_t CalculatePadding(const std::size_t baseAddress, const std::size_t alignment) {
			const std::size_t multiplier = (baseAddress / alignment) + 1;
			const std::size_t alignedAddress = multiplier * alignment;
			const std::size_t padding = alignedAddress - baseAddress;
			return padding;
		}

		static const std::size_t CalculatePaddingWithHeader(const std::size_t baseAddress, const std::size_t alignment, const std::size_t headerSize) {
			std::size_t padding = CalculatePadding(baseAddress, alignment);
			std::size_t neededSpace = headerSize;

			if (padding < neededSpace){
				// Header does not fit - Calculate next aligned address that header fits
				neededSpace -= padding;

				// How many alignments I need to fit the header
				if(neededSpace % alignment > 0){
					padding += alignment * (1+(neededSpace / alignment));
				}else {
					padding += alignment * (neededSpace / alignment);
				}
			}

			return padding;
		}
	};
}



#endif /* SRC_MEMORY_UTILS_H_ */
