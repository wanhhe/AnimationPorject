#define _CRT_SECURE_NO_WARNINGS
#include "Sample.h"
//#include "mat4.h"
//#include "quat.h"
//#include "Uniform.h"
//#include "Draw.h"
#include "GLTFLoader.h"
#include <iostream>

//void Sample::Initialize() {
//	mRotation = 0.0f;
//	mShader = new Shader("./Shaders/static.vert", "./Shaders/lit.frag");
//	mDisplayTexture = new Texture("./Assets/uv.png");
//
//	mVertexPositions = new Attribute<vec3>();
//	mVertexNormals = new Attribute<vec3>();
//	mVertexTexCoords = new Attribute<vec2>();
//	mIndexBuffer = new IndexBuffer();
//
//	std::vector<vec3> positions;
//	positions.push_back(vec3(-1, -1, 0));
//	positions.push_back(vec3(-1, 1, 0));
//	positions.push_back(vec3(1, -1, 0));
//	positions.push_back(vec3(1, 1, 0));
//	mVertexPositions->Set(positions);
//
//	std::vector<vec3> normals;
//	normals.resize(4, vec3(0, 0, 1));
//	mVertexNormals->Set(normals);
//
//	std::vector<vec2> uvs;
//	uvs.push_back(vec2(0, 0));
//	uvs.push_back(vec2(0, 1));
//	uvs.push_back(vec2(1, 0));
//	uvs.push_back(vec2(1, 1));
//	mVertexTexCoords->Set(uvs);
//
//	std::vector<unsigned int> indices;
//	indices.push_back(0);
//	indices.push_back(1);
//	indices.push_back(2);
//	indices.push_back(2);
//	indices.push_back(1);
//	indices.push_back(3);
//	mIndexBuffer->Set(indices);
//}

void Sample::Initialize() {
	cgltf_data* gltf = LoadGLTFFile("./Assets/narmaya.gltf");
	//cgltf_data* gltf = LoadGLTFFile("./Assets/Woman.gltf");
	mRestPose = LoadRestPose(gltf);
	mClips = LoadAnimationClips(gltf);
	FreeGLTFFile(gltf);

	mRestPoseVisual = new DebugDraw();
	mRestPoseVisual->FromPose(mRestPose);
	mRestPoseVisual->UpdateOpenGLBuffers();

	mCurrentClip = 0;
	mCurrentPose = mRestPose;

	mCurrentPoseVisual = new DebugDraw();
	mCurrentPoseVisual->FromPose(mCurrentPose);
	mCurrentPoseVisual->UpdateOpenGLBuffers();

	unsigned int numUIClips = (unsigned int)mClips.size();
	for (unsigned int i = 0; i < numUIClips; ++i) {
		if (mClips[i].GetName() == "GBVS_Narmaya|Scene|Base Layer") {
			mCurrentClip = i;
			break;
		}
	}
}

//void Sample::Update(float inDeltaTime) {
//	mRotation += inDeltaTime * 45.0f;
//	while (mRotation > 360.0f) {
//		mRotation -= 360.0f;
//	}
//}

void Sample::Update(float inDeltaTime) {
	mPlaybackTime = mClips[mCurrentClip].Sample(mCurrentPose, mPlaybackTime + inDeltaTime);
	mCurrentPoseVisual->FromPose(mCurrentPose);
}

//void Sample::Render(float inAspectRatio) {
//	mat4 projection = perspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
//	mat4 view = lookAt(vec3(0, 0, -5), vec3(0, 0, 0), vec3(0, 1, 0));
	//mat4 model = quatToMat4(angleAxis(mRotation * DEG2RAD, vec3(0, 0, 1)));
//
//	mShader->Bind();
//
//	mVertexPositions->BindTo(mShader->GetAttributes("position"));
//	mVertexNormals->BindTo(mShader->GetAttributes("normal"));
//	mVertexTexCoords->BindTo(mShader->GetAttributes("texCoord"));
//
//	Uniform<mat4>::Set(mShader->GetUniforms("model"), model);
//	Uniform<mat4>::Set(mShader->GetUniforms("view"), view);
//	Uniform<mat4>::Set(mShader->GetUniforms("projection"), projection);
//
//	Uniform<vec3>::Set(mShader->GetUniforms("light"), vec3(0, 0, 1));
//
//	mDisplayTexture->Set(mShader->GetUniforms("tex0"), 0);
//
//	Draw(*mIndexBuffer, DrawMode::Triangles);
//
//	mDisplayTexture->UnSet(0);
//
//	mVertexPositions->UnBindFrom(mShader->GetAttributes("position"));
//	mVertexNormals->UnBindFrom(mShader->GetAttributes("normal"));
//	mVertexTexCoords->UnBindFrom(mShader->GetAttributes("texCoord"));
//
//	mShader->UnBind();
//}

void Sample::Render(float inAspectRatio) {
	mat4 projection = perspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
	mat4 view = lookAt(vec3(0, 4, 7), vec3(0, 4, 0), vec3(0, 1, 0));
	//mat4 mvp = projection * view; // 不做仿射变换
	mat4 model = mat4(50000, 0, 0, 0, 0, 50000, 0, 0, 0, 0, 50000, 0, 0, 0, 0, 1);
	mat4 mvp = projection * view * model;

	mRestPoseVisual->Draw(DebugDrawMode::Lines, vec3(1, 0, 0), mvp); // 画原来的样子，进行对比
	mCurrentPoseVisual->UpdateOpenGLBuffers();
	mCurrentPoseVisual->Draw(DebugDrawMode::Lines, vec3(0, 0, 1), mvp);
}

//void Sample::Shutdown() {
//	delete mShader;
//	delete mDisplayTexture;
//	delete mVertexPositions;
//	delete mVertexNormals;
//	delete mVertexTexCoords;
//	delete mIndexBuffer;
//}

void Sample::Shutdown() {
	delete mCurrentPoseVisual;
	delete mRestPoseVisual;
	mClips.clear();
}