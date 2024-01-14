#ifndef _H_DRAW_
#define _H_DRAW_

#include "IndexBuffer.h"

enum class DrawMode {
	Points,
	LineStrip,
	LineLoop,
	Lines,
	Triangles,
	TriangleStrip,
	TriangleFan
};

void Draw(IndexBuffer& inIndexBuffer, DrawMode mode);
void Draw(unsigned int vertexCount, DrawMode mode);

// ∂‡ µ¿˝‰÷»æ
void DrawInstance(IndexBuffer& inIndexBuffer, DrawMode mode, unsigned int instanceCount);
void DrawInstance(unsigned int vertexCount, DrawMode mode, unsigned int numInstances);

#endif // !_H_DRAW_

