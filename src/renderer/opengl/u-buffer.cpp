#include "u-buffer.h"

namespace Renderer {

	UBufferData::UBufferData()
		: count (1)
	{}

	UBufferData::UBufferData(std::vector<std::size_t> sizes)
		: count(1)
		, sizes(sizes)
	{}

	UBufferData::UBufferData(const uint count, std::vector<std::size_t> sizes)
		: count(count)
		, sizes(sizes)
	{}

	UBufferData::UBufferData(const UBufferData& other)
		: count(other.count)
		, sizes(other.sizes)
	{}

	const std::vector<std::size_t>& UBufferData::getSizes() const
	{
		return sizes;
	}

	const uint UBufferData::getCount() const
	{
		return count;
	}

	
	
	UBUfferOffset::UBUfferOffset() {}
	UBUfferOffset::UBUfferOffset(const UBufferData& data, size_t& globalOffset)
		: count(data.getCount())
	{
		const std::vector<std::size_t>& sizes = data.getSizes();

		for (uint i = 0; i < count; i++) {
			for (const std::size_t size : sizes) {
				size_t newSize = size;
				if (newSize == 12) {
					newSize = 16;
				}

				std::pair<std::size_t, std::size_t> offset = std::make_pair(globalOffset, size);
				offsets.push_back(offset);
				globalOffset+= newSize;
			}
		}
	}

	UBUfferOffset::UBUfferOffset(const UBUfferOffset& other)
		: count(other.count)
		, offsets(other.offsets)
	{}

	const std::pair<std::size_t, std::size_t>& UBUfferOffset::get(uint index) const
	{
		return get(0, index);
	}

	const std::pair<std::size_t, std::size_t>& UBUfferOffset::get(uint offsetIndex, uint index) const
	{
		return offsets[offsetIndex * count + index];
	}



	UBuffer::UBuffer() {}
	UBuffer::~UBuffer() {}

	void UBuffer::init(std::vector<std::size_t> sizes, GLuint mountPoint)
	{
		UBufferData data(1, sizes);
		UBufferMapSizes mapSizes;
		mapSizes[UBUFFER_DEFAULT_GROUP_NAME] = data;

		init(mapSizes, mountPoint);
	}

	void UBuffer::init(UBufferMapSizes mapSizes, GLuint mountPoint)
	{
		std::size_t globalOffsetSize = 0;

		for (auto it = mapSizes.begin(); it != mapSizes.end(); it++) {
			const std::string& groupName = it->first;
			const UBufferData& sizesData = it->second;

			UBUfferOffset offset(sizesData, globalOffsetSize);
			offsets_[groupName] = offset;
		}

		setDataSize(globalOffsetSize);

		glGenBuffers(1, &uboHandle_);
		glBindBuffer(GL_UNIFORM_BUFFER, uboHandle_);
		glBufferData(GL_UNIFORM_BUFFER, getDataSize(), NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferRange(GL_UNIFORM_BUFFER, mountPoint, uboHandle_, 0, getDataSize());
	}

	void UBuffer::set(short int index, const GLvoid * data)
	{
		set(UBUFFER_DEFAULT_GROUP_NAME, index, data);
	}

	void UBuffer::set(std::string group, short int index, const GLvoid * data)
	{
		set(group, 0, index, data);
	}

	void UBuffer::set(std::string group, short int offsetIndex, short int index, const GLvoid * data)
	{
		std::unordered_map<std::string, UBUfferOffset>::const_iterator it = offsets_.find(group);

		if (it == offsets_.end()) {
			console::warn("ubuffer offset not found");
			return;
		}

		const UBUfferOffset& offset = it->second;
		const std::pair<std::size_t, std::size_t>& offsetInfo = offset.get(index);

		glBufferSubData(GL_UNIFORM_BUFFER, offsetInfo.first, offsetInfo.second, data);
	}

	void UBuffer::setDataSize(size_t size)
	{
		dataSize_ = size;
	}

	const size_t& UBuffer::getDataSize()
	{
		return dataSize_;
	}

	void UBuffer::use()
	{
		// if (isUsed_ == true) {
		// 	console::warn("u-buffer already used ", uboHandle_);
		// 	return;
		// }

		isUsed_ = true;

		glBindBuffer(GL_UNIFORM_BUFFER, uboHandle_);
	}

	void UBuffer::nouse()
	{
		// if (isUsed_ == false) {
		// 	console::warn("u-buffer is not used ", uboHandle_);
		// 	return;
		// }

		isUsed_ = false;

		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

}
