#include "u-buffer.h"

namespace Renderer {

UBuffer::UBuffer()
{

}

UBuffer::~UBuffer()
{

}

void UBuffer::init(std::vector<std::size_t> sizes, GLuint mountPoint)
{
	offsets_.reserve(sizes.size());

	std::size_t dataSize = 0;
	std::size_t offsetSize = 0;
	short int index = 0;
	for (const std::size_t size : sizes)
	{
		std::pair<std::size_t, std::size_t> offset = std::make_pair(offsetSize, size);
		offsets_.insert(std::make_pair(index, offset));

		index++;
		offsetSize+= size;
		dataSize+= size;
	}

	dataSize_ = dataSize;

	glGenBuffers(1, &ubo_);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_);
	glBufferData(GL_UNIFORM_BUFFER, dataSize_, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(GL_UNIFORM_BUFFER, mountPoint, ubo_, 0, dataSize_);
}

void UBuffer::set(short int index, const GLvoid * data)
{
	UBUfferOffset::const_iterator it = offsets_.find(index);

	if (it == offsets_.end()) {
		console::warn("ubuffer offset not found");
		return;
	}

	std::size_t offset = it->second.first;
	std::size_t size = it->second.second;

	glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
}

void UBuffer::use()
{
	if (isUsed_) {
		console::warn("u-buffer already used");
		return;
	}

	isUsed_ = true;

	glBindBuffer(GL_UNIFORM_BUFFER, ubo_);
}

void UBuffer::nouse()
{
	if (!isUsed_) {
		console::warn("u-buffer is not used");
		return;
	}

	isUsed_ = false;

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

}
