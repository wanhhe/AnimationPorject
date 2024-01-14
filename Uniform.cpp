#include "Uniform.h"
#include "glad.h"
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "quat.h"
#include "mat4.h"

template Uniform<int>;
template Uniform<ivec4>;
template Uniform<ivec2>;
template Uniform<float>;
template Uniform<vec2>;
template Uniform<vec3>;
template Uniform<vec4>;
template Uniform<quat>;
template Uniform<mat4>;

template<typename T>
void Uniform<T>::Set(unsigned int slot, const T& value) {
	Set(slot, (T*)&value, 1);
}

template<typename T>
void Uniform<T>::Set(unsigned int slot, std::vector<T>& inputArray) {
	Set(slot, &inputArray[0], (unsigned int)inputArray.size());
}

template<>
void Uniform<mat4>::Set(unsigned int slot, mat4* inputArray, int length) {
	// location 要更改的矩阵个数 是否要转置 开始的指针
	glUniformMatrix4fv(slot, (GLsizei)length, false, (float*)&inputArray[0]);
}

template<>
void Uniform<int>::Set(unsigned int slot, int* inputArray, int length) {
	// location 要更改的元素个数 一个具有count个数值的数组指针
	glUniform1iv(slot, (GLsizei)length, (int*)&inputArray[0]);
}

template<>
void Uniform<ivec2>::Set(unsigned int slot, ivec2* inputArray, int length) {
	glUniform4iv(slot, (GLsizei)length, (int*)&inputArray[0]);
}

template<>
void Uniform<ivec4>::Set(unsigned int slot, ivec4* inputArray, int length) {
	glUniform2iv(slot, (GLsizei)length, (int*)&inputArray[0]);
}

template<>
void Uniform<float>::Set(unsigned int slot, float* inputArray, int length) {
	glUniform1fv(slot, (GLsizei)length, (float*)&inputArray[0]);
}

template<>
void Uniform<vec2>::Set(unsigned int slot, vec2* inputArray, int length) {
	glUniform2fv(slot, (GLsizei)length, (float*)&inputArray[0]);
}

template<>
void Uniform<vec3>::Set(unsigned int slot, vec3* inputArray, int length) {
	glUniform3fv(slot, (GLsizei)length, (float*)&inputArray[0]);
}

template<>
void Uniform<vec4>::Set(unsigned int slot, vec4* inputArray, int length) {
	glUniform4fv(slot, (GLsizei)length, (float*)&inputArray[0]);
}

template<>
void Uniform<quat>::Set(unsigned int slot, quat* inputArray, int length) {
	glUniform4fv(slot, (GLsizei)length, (float*)&inputArray[0]);
}

