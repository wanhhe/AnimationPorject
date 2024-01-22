#define _CRT_SECURE_NO_WARNINGS
#include "Sample.h"
//#include "mat4.h"
//#include "quat.h"
//#include "Uniform.h"
//#include "Draw.h"
#include "GLTFLoader.h"
#include <iostream>

#include "Uniform.h"

#include "RearrangeBones.h"

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

//void Sample::Initialize() {
//	cgltf_data* gltf = LoadGLTFFile("./Assets/narmaya.gltf");
//	//cgltf_data* gltf = LoadGLTFFile("./Assets/Woman.gltf");
//	mRestPose = LoadRestPose(gltf);
//	mClips = LoadAnimationClips(gltf);
//	FreeGLTFFile(gltf);
//
//	mRestPoseVisual = new DebugDraw();
//	mRestPoseVisual->FromPose(mRestPose);
//	mRestPoseVisual->UpdateOpenGLBuffers();
//
//	mCurrentClip = 0;
//	mCurrentPose = mRestPose;
//
//	mCurrentPoseVisual = new DebugDraw();
//	mCurrentPoseVisual->FromPose(mCurrentPose);
//	mCurrentPoseVisual->UpdateOpenGLBuffers();
//
//	unsigned int numUIClips = (unsigned int)mClips.size();
//	for (unsigned int i = 0; i < numUIClips; ++i) {
//		if (mClips[i].GetName() == "GBVS_Narmaya|Scene|Base Layer") {
//			mCurrentClip = i;
//			break;
//		}
//	}
//}

void Sample::Initialize() {
	cgltf_data* gltf = LoadGLTFFile("Assets/GBVS_Narmaya.gltf");
	mCPUMeshes = LoadMeshes(gltf);
	mSkeleton = LoadSkeleton(gltf);
	std::vector<Clip> clips = LoadAnimationClips(gltf);
	mClips.resize(clips.size());
	for (unsigned int i = 0; i < (unsigned int)clips.size(); i++) {
		mClips[i] = OptimizeClip(clips[i]);
	}
	FreeGLTFFile(gltf);

	// 调整骨骼顺序
	BoneMap bones = RearrangeSkeleton(mSkeleton);
	for (unsigned int i = 0; i < (unsigned int)mCPUMeshes.size(); i++) {
		// 对每个mesh也要调整
		RearrangeMesh(mCPUMeshes[i], bones);
	}
	for (unsigned int i = 0; i < mClips.size(); i++) {
		// 由于骨骼id变了，TransformTrack中的id也就变了，需要调整
		RearrangeFastClip(mClips[i], bones);
	}

	mGPUMeshes = mCPUMeshes;
	for (unsigned int i = 0, size = (unsigned int)mGPUMeshes.size(); i < size; ++i) {
		mGPUMeshes[i].UpdateOpenGLBuffers();
	}

	mStaticShader = new Shader("Shaders/static.vert", "Shaders/lit.frag");
	//mSkinnedShader = new Shader("Shaders/skinned.vert", "Shaders/lit.frag");
	mSkinnedShader = new Shader("Shaders/preskinned.vert", "Shaders/lit.frag");
	//mSkinnedShader = new Shader("Shaders/static.vert", "Shaders/lit.frag");
	mDiffuseTexture = new Texture("Assets/NRM_base.png");

	mGPUAnimInfo.mAnimatedPose = mSkeleton.GetRestPose();
	mGPUAnimInfo.mPosePalette.resize(mSkeleton.GetRestPose().Size());
	mCPUAnimInfo.mAnimatedPose = mSkeleton.GetRestPose();
	mCPUAnimInfo.mPosePalette.resize(mSkeleton.GetRestPose().Size());
	mGPUAnimInfo.mModel.position = vec3(-2, 0, 0);
	mCPUAnimInfo.mModel.position = vec3(2, 0, 0);
	mGPUAnimInfo.mModel.scale = vec3(200, 200, 200);
	mCPUAnimInfo.mModel.scale = vec3(400, 400, 400);

	std::cout << "pose: " << mSkeleton.GetRestPose().Size() << ";\n";

	unsigned int numUIClips = (unsigned int)mClips.size();
	for (unsigned int i = 0; i < numUIClips; i++) {
		if (mClips[i].GetName() == "Walking") {
			mCPUAnimInfo.mClip = i;
		}
		else if (mClips[i].GetName() == "Running") {
			mGPUAnimInfo.mClip = i;
		}
	}
}

//void Sample::Update(float inDeltaTime) {
//	mRotation += inDeltaTime * 45.0f;
//	while (mRotation > 360.0f) {
//		mRotation -= 360.0f;
//	}
//}

//void Sample::Update(float inDeltaTime) {
//	mPlaybackTime = mClips[mCurrentClip].Sample(mCurrentPose, mPlaybackTime + inDeltaTime);
//	mCurrentPoseVisual->FromPose(mCurrentPose);
//}

//void Sample::Update(float inDeltaTime) {
//	mCPUAnimInfo.mPlayback = mClips[mCPUAnimInfo.mClip].Sample(mCPUAnimInfo.mAnimatedPose, mCPUAnimInfo.mPlayback + inDeltaTime);
//	mGPUAnimInfo.mPlayback = mClips[mGPUAnimInfo.mClip].Sample(mGPUAnimInfo.mAnimatedPose, mGPUAnimInfo.mPlayback + inDeltaTime);
//
//	unsigned int size = (unsigned int)mCPUMeshes.size();
//	for (unsigned int i = 0; i < size; i++) {
//		mCPUMeshes[i].CPUSkin(mSkeleton, mCPUAnimInfo.mAnimatedPose);
//	}
//
//	mGPUAnimInfo.mAnimatedPose.GetMatrixPalette(mGPUAnimInfo.mPosePalette);
//}

// 改进
void Sample::Update(float inDeltaTime) {
	mCPUAnimInfo.mPlayback = mClips[mCPUAnimInfo.mClip].Sample(mCPUAnimInfo.mAnimatedPose, mCPUAnimInfo.mPlayback + inDeltaTime);
	mGPUAnimInfo.mPlayback = mClips[mGPUAnimInfo.mClip].Sample(mGPUAnimInfo.mAnimatedPose, mGPUAnimInfo.mPlayback + inDeltaTime);

	mCPUAnimInfo.mAnimatedPose.GetMatrixPalette(mCPUAnimInfo.mPosePalette);
	mGPUAnimInfo.mAnimatedPose.GetMatrixPalette(mGPUAnimInfo.mPosePalette);

	// 获得动画变换矩阵
	unsigned int size = (unsigned int)mCPUAnimInfo.mPosePalette.size();;
	for (unsigned int i = 0; i < size; i++) {
		mCPUAnimInfo.mPosePalette[i] = mCPUAnimInfo.mPosePalette[i] * mSkeleton.GetInvBindPose()[i];
		mGPUAnimInfo.mPosePalette[i] = mGPUAnimInfo.mPosePalette[i] * mSkeleton.GetInvBindPose()[i];
	}
	
	size = (unsigned int)mCPUMeshes.size();
	for (unsigned int i = 0; i < size; i++) {
		mCPUMeshes[i].CPUSkin(mCPUAnimInfo.mPosePalette);
	}
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

//void Sample::Render(float inAspectRatio) {
//	mat4 projection = perspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
//	mat4 view = lookAt(vec3(0, 4, 7), vec3(0, 4, 0), vec3(0, 1, 0));
//	//mat4 mvp = projection * view; // 不做仿射变换
//	mat4 model = mat4(50000, 0, 0, 0, 0, 50000, 0, 0, 0, 0, 50000, 0, 0, 0, 0, 1);
//	mat4 mvp = projection * view * model;
//
//	mRestPoseVisual->Draw(DebugDrawMode::Lines, vec3(1, 0, 0), mvp); // 画原来的样子，进行对比
//	mCurrentPoseVisual->UpdateOpenGLBuffers();
//	mCurrentPoseVisual->Draw(DebugDrawMode::Lines, vec3(0, 0, 1), mvp);
//}

void Sample::Render(float inAspectRatio) {
	mat4 projection = perspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
	mat4 view = lookAt(vec3(0, 5, 7), vec3(0, 3, 0), vec3(0, 1, 0));
	//mat4 view = lookAt(vec3(10, 5, 7), vec3(0, 3, 0), vec3(0, 1, 0));
	mat4 model;

	// CPU 蒙皮
	model = transformToMat4(mCPUAnimInfo.mModel);
	mStaticShader->Bind();
	Uniform<mat4>::Set(mStaticShader->GetUniform("model"), model);
	Uniform<mat4>::Set(mStaticShader->GetUniform("view"), view);
	Uniform<mat4>::Set(mStaticShader->GetUniform("projection"), projection);
	Uniform<vec3>::Set(mStaticShader->GetUniform("light"), vec3(1, 1, 1));

	mDiffuseTexture->Set(mStaticShader->GetUniform("tex0"), 0);
	for (unsigned int i = 0, size = (unsigned int)mCPUMeshes.size(); i < size; ++i) {
		mCPUMeshes[i].Bind(mStaticShader->GetAttribute("position"), mStaticShader->GetAttribute("normal"), mStaticShader->GetAttribute("texCoord"), -1, -1);
		mCPUMeshes[i].Draw();
		mCPUMeshes[i].UnBind(mStaticShader->GetAttribute("position"), mStaticShader->GetAttribute("normal"), mStaticShader->GetAttribute("texCoord"), -1, -1);
	}
	mDiffuseTexture->UnSet(0);
	mStaticShader->UnBind();

	// GPU Skinned Mesh
	model = transformToMat4(mGPUAnimInfo.mModel);
	mSkinnedShader->Bind();
	Uniform<mat4>::Set(mSkinnedShader->GetUniform("model"), model);
	Uniform<mat4>::Set(mSkinnedShader->GetUniform("view"), view);
	Uniform<mat4>::Set(mSkinnedShader->GetUniform("projection"), projection);
	Uniform<vec3>::Set(mSkinnedShader->GetUniform("light"), vec3(1, 1, 1));

	//Uniform<mat4>::Set(mSkinnedShader->GetUniform("pose"), mGPUAnimInfo.mPosePalette);
	//Uniform<mat4>::Set(mSkinnedShader->GetUniform("invBindPose"), mSkeleton.GetInvBindPose());
	// 改进
	Uniform<mat4>::Set(mSkinnedShader->GetUniform("animated"), mGPUAnimInfo.mPosePalette);

	mDiffuseTexture->Set(mSkinnedShader->GetUniform("tex0"), 0);
	for (unsigned int i = 0, size = (unsigned int)mGPUMeshes.size(); i < size; ++i) {
		mGPUMeshes[i].Bind(mSkinnedShader->GetAttribute("position"), mSkinnedShader->GetAttribute("normal"), mSkinnedShader->GetAttribute("texCoord"), mSkinnedShader->GetAttribute("weights"), mSkinnedShader->GetAttribute("joints"));
		mGPUMeshes[i].Draw();
		mGPUMeshes[i].UnBind(mSkinnedShader->GetAttribute("position"), mSkinnedShader->GetAttribute("normal"), mSkinnedShader->GetAttribute("texCoord"), mSkinnedShader->GetAttribute("weights"), mSkinnedShader->GetAttribute("joints"));
	}
	mDiffuseTexture->UnSet(0);
	mSkinnedShader->UnBind();
}

//void Sample::Shutdown() {
//	delete mShader;
//	delete mDisplayTexture;
//	delete mVertexPositions;
//	delete mVertexNormals;
//	delete mVertexTexCoords;
//	delete mIndexBuffer;
//}

//void Sample::Shutdown() {
//	delete mCurrentPoseVisual;
//	delete mRestPoseVisual;
//	mClips.clear();
//}

void Sample::Shutdown() {
	delete mDiffuseTexture;
	delete mStaticShader;
	delete mSkinnedShader;
	mClips.clear();
	mCPUMeshes.clear();
	mGPUMeshes.clear();
}