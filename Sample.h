#ifndef _H_SAMPLE_
#define _H_SAMPLE_

#include "Application.h"
//#include "Shader.h"
//#include "Attribute.h"
//#include "IndexBuffer.h"
//#include "Texture.h"

//#include "DebugDraw.h"
#include <vector>
#include "Pose.h"
#include "Clip.h"

#include "Skeleton.h"
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"

//#define DEG2RAD 0.0174533f

struct AnimationInstance {
	Pose mAnimatedPose;
	std::vector<mat4> mPosePalette;
	unsigned int mClip;
	float mPlayback;
	Transform mModel;

	inline AnimationInstance() : mClip(0), mPlayback(0.0f) {}
};

class Sample: public Application {
protected:
	//Shader* mShader;
	//Attribute<vec3>* mVertexPositions;
	//Attribute<vec3>* mVertexNormals;
	//Attribute<vec2>* mVertexTexCoords;
	//IndexBuffer* mIndexBuffer;
	//Texture* mDisplayTexture;
	//float mRotation;

	//Pose mRestPose;
	//Pose mCurrentPose;
	std::vector<FastClip> mClips;
	//unsigned int mCurrentClip;
	//float mPlaybackTime;
	//DebugDraw* mRestPoseVisual;
	//DebugDraw* mCurrentPoseVisual;

	Texture* mDiffuseTexture;
	Shader* mStaticShader;
	Shader* mSkinnedShader;
	std::vector<Mesh> mCPUMeshes;
	std::vector<Mesh> mGPUMeshes;
	Skeleton mSkeleton;

	AnimationInstance mGPUAnimInfo;
	AnimationInstance mCPUAnimInfo;

public:
	void Initialize();
	void Update(float inDeltaTime);
	void Render(float inAspectRatio);
	void Shutdown();
};

#endif