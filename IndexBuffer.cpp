#include "IndexBuffer.h"
#include "glad.h"

IndexBuffer::IndexBuffer() {
	glGenBuffers(1, &mHandle);
	mCount = 0;
}

IndexBuffer::~IndexBuffer() {
	glDeleteBuffers(1, &mHandle);
}

unsigned int IndexBuffer::Count() {
	return mCount;
}

unsigned int IndexBuffer::GetHandle() {
	return mHandle;
}

void IndexBuffer::Set(unsigned int* inputArray, unsigned int len) {
	mCount = len;

	unsigned int size = sizeof(unsigned int);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mHandle);
	// 第二个参数指定传输数据的大小(以字节为单位)
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * mCount, inputArray, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::Set(std::vector<unsigned int>& input) {
	Set(&input[0], (unsigned int)input.size());
}