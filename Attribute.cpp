#include "Attribute.h"
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "quat.h"
#include "glad.h"

template Attribute<int>;
template Attribute<float>;
template Attribute<vec2>;
template Attribute<vec3>;
template Attribute<vec4>;
template Attribute<ivec4>;
template Attribute<quat>;

template<typename T>
Attribute<T>::Attribute() {
	glGenBuffers(1, &mHandle);
	mCount = 0;
}

template<typename T>
Attribute<T>::~Attribute() {
	glDeleteBuffers(1, &mHandle);
}

template<typename T>
unsigned int Attribute<T>::Count() {
	return mCount;
}

template<typename T>
unsigned int Attribute<T>::GetHandle() {
	return mHandle;
}

template<typename T>
void Attribute<T>::Set(T* inputArray, unsigned int arrayLength) {
	mCount = arrayLength;
	unsigned int size = sizeof(T);

	glBindBuffer(GL_ARRAY_BUFFER, mHandle); // 绑定缓冲对象
	glBufferData(GL_ARRAY_BUFFER, size * mCount, inputArray, GL_STREAM_DRAW); // 传数据  第二个参数指定传输数据的大小(以字节为单位)
	glBindBuffer(GL_ARRAY_BUFFER, 0); // 送完数据后恢复初态
}

template<typename T>
void Attribute<T>::Set(std::vector<T>& input) {
	Set(&input[0], (unsigned int)input.size());
}

template<>
void Attribute<int>::SetAttribPointer(unsigned int slot) {
	// 告诉OpenGL如何解析顶点数据
	glVertexAttribIPointer(slot, 1, GL_INT, 0, (void*)0); // 步长设置为0就由OpenGL来决定步长。要求数据必须要紧密排列
}

template<>
void Attribute<ivec4>::SetAttribPointer(unsigned int slot) {
	glVertexAttribIPointer(slot, 4, GL_INT, 0, (void*)0);
}

template<>
void Attribute<float>::SetAttribPointer(unsigned int slot) {
	glVertexAttribPointer(slot, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

template<>
void Attribute<vec2>::SetAttribPointer(unsigned int slot) {
	glVertexAttribPointer(slot, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

template<>
void Attribute<vec3>::SetAttribPointer(unsigned int slot) {
	glVertexAttribPointer(slot, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

template<>
void Attribute<vec4>::SetAttribPointer(unsigned int slot) {
	glVertexAttribPointer(slot, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

template<>
void Attribute<quat>::SetAttribPointer(unsigned int slot) {
	glVertexAttribPointer(slot, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

template<typename T>
void Attribute<T>::BindTo(unsigned int slot) {
	glBindBuffer(GL_ARRAY_BUFFER, mHandle); // 需要设置mHandle的VAO，则要先绑定
	glEnableVertexAttribArray(slot); // 开启该位置的VAO
	SetAttribPointer(slot);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

template<typename T>
void Attribute<T>::UnBindFrom(unsigned int slot) {
	glBindBuffer(GL_ARRAY_BUFFER, mHandle);
	glDisableVertexAttribArray(slot); // 关闭该位置的VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}