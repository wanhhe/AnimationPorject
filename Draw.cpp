#include "Draw.h"
#include "glad.h"
#include <iostream>

static GLenum DrawModeToGLenum(DrawMode input) {
	switch (input)
	{
	case DrawMode::Points:
		return GL_POINTS;
	case DrawMode::LineStrip:
		return GL_LINE_STRIP;
	case DrawMode::LineLoop:
		return GL_LINE_LOOP;
	case DrawMode::Lines:
		return GL_LINE;
	case DrawMode::Triangles:
		return GL_TRIANGLES;
	case DrawMode::TriangleStrip:
		return GL_TRIANGLE_STRIP;
	case DrawMode::TriangleFan:
		return GL_TRIANGLE_FAN;
	}
	std::cout << "DrawModeToGLEnum unreachable code hit\n";
	return 0;
}

void Draw(unsigned int vertexCount, DrawMode mode) {
	// 第二个参数指定了顶点数组的起始索引，最后一个参数指定打算绘制多少个顶点
	glDrawArrays(DrawModeToGLenum(mode), 0, vertexCount);
}

void DrawInstance(unsigned int vertexCount, DrawMode mode, unsigned int numInstances) {
	glDrawArraysInstanced(DrawModeToGLenum(mode), 0, vertexCount, numInstances);
}

void Draw(IndexBuffer& inIndexBuffer, DrawMode mode) {
	unsigned int handle = inIndexBuffer.GetHandle();
	unsigned int numIndices = inIndexBuffer.Count();
	
	// 使用时需绑定缓冲
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
	glDrawElements(DrawModeToGLenum(mode), numIndices, GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void DrawInstance(IndexBuffer& inIndexBuffer, DrawMode mode, unsigned int instanceCount) {
	unsigned int handle = inIndexBuffer.GetHandle();
	unsigned int numIndices = inIndexBuffer.Count();

	// 使用时需绑定缓冲
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
	glDrawElementsInstanced(DrawModeToGLenum(mode), numIndices, GL_UNSIGNED_INT, 0, instanceCount);
}

