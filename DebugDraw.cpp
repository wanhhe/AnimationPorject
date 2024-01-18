#include "DebugDraw.h"
#include "Uniform.h"
#include "Draw.h"
#include <iostream>

DebugDraw::DebugDraw() {
	mAttributes = new Attribute<vec3>();

	mShader = new Shader(
		"#version 330 core\n"
		"uniform mat4 mvp;\n"
		"in vec3 position;\n"
		"void main() {\n"
		"	gl_Position = mvp * vec4(position, 1.0);\n"
		"}"
		,
		"#version 330 core\n"
		"uniform vec3 color;\n"
		"out vec4 FragColor;\n"
		"void main() {\n"
		"	FragColor = vec4(color, 1);\n"
		"}"
	);
}

DebugDraw::DebugDraw(unsigned int size) {
	mAttributes = new Attribute<vec3>();

	mShader = new Shader(
		"#version 330 core\n"
		"uniform mat4 mvp;\n"
		"in vec3 position;\n"
		"void main() {\n"
		"	gl_Position = mvp * vec4(position, 1.0);\n"
		"}"
		,
		"#version 330 core\n"
		"uniform vec3 color;\n"
		"out vec4 FragColor;\n"
		"void main() {\n"
		"	FragColor = vec4(color, 1);\n"
		"}"
	);

	Resize(size);
}
DebugDraw::~DebugDraw() {
	delete mAttributes;
	delete mShader;
}

unsigned int DebugDraw::Size() {
	return (unsigned int)mPoints.size();
}

void DebugDraw::Resize(unsigned int size) {
	mPoints.resize(size);
}

vec3& DebugDraw::operator[](unsigned int index) {
	return mPoints[index];
}

void DebugDraw::Push(const vec3& v) {
	mPoints.push_back(v);
}

void DebugDraw::FromPose(Pose& pose) {
	unsigned int numJoints = pose.Size();
	unsigned int requireVerts = 0;
	for (unsigned int i = 0; i < numJoints; i++) {
		if (pose.GetParent(i) < 0) { // 如果没有父节点了
			continue;
		}

		requireVerts += 2; // 若不是父节点，则需该结点和其父节点一起绘制一段骨骼
	}

	mPoints.resize(requireVerts);
	for (unsigned int i = 0; i < numJoints; i++) {
		int parent = pose.GetParent(i);
		if (parent < 0) { // 如果没有父节点了
			continue;
		}

		mPoints.push_back(pose.GetGlobalTransform(i).position);
		mPoints.push_back(pose.GetGlobalTransform(parent).position);
	}
}

void DebugDraw::UpdateOpenGLBuffers() {
	mAttributes->Set(mPoints);
}

void DebugDraw::Draw(DebugDrawMode mode, const vec3& color, const mat4& mvp) {
	mShader->Bind();
	Uniform<mat4>::Set(mShader->GetUniform("mvp"), mvp);
	Uniform<vec3>::Set(mShader->GetUniform("color"), color);
	mAttributes->BindTo(mShader->GetAttribute("position"));

	if (mode == DebugDrawMode::Lines) {
		::Draw(Size(), DrawMode::Lines); // ::Draw表示调用全局函数
	}
	else if (mode == DebugDrawMode::Loop) {
		::Draw(Size(), DrawMode::LineLoop);
	}
	else if (mode == DebugDrawMode::Strip) {
		::Draw(Size(), DrawMode::LineStrip);
	}
	else if (mode == DebugDrawMode::Points) {
		::Draw(Size(), DrawMode::Points);
	}
	mAttributes->UnBindFrom(mShader->GetAttribute("position"));
	mShader->UnBind();
}