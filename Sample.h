#ifndef _H_SAMPLE_
#define _H_SAMPLE_

#include "Application.h"
//#include "Shader.h"
//#include "Attribute.h"
//#include "IndexBuffer.h"
//#include "Texture.h"
#include "DebugDraw.h"
#include <vector>
#include "Pose.h"
#include "Clip.h"

//#define DEG2RAD 0.0174533f

class Sample: public Application {
protected:
	//Shader* mShader;
	//Attribute<vec3>* mVertexPositions;
	//Attribute<vec3>* mVertexNormals;
	//Attribute<vec2>* mVertexTexCoords;
	//IndexBuffer* mIndexBuffer;
	//Texture* mDisplayTexture;
	//float mRotation;
	Pose mRestPose;
	Pose mCurrentPose;
	std::vector<Clip> mClips;
	unsigned int mCurrentClip;
	float mPlaybackTime;
	DebugDraw* mRestPoseVisual;
	DebugDraw* mCurrentPoseVisual;

public:
	void Initialize();
	void Update(float inDeltaTime);
	void Render(float inAspectRatio);
	void Shutdown();
};

#endif