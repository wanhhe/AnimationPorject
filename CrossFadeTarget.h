#ifndef _H_CROSSFADETARGET
#define _H_CROSSFADETARGET

#include "Pose.h"
#include "Clip.h"

struct CrossFadeTarget
{
	Pose mPose; // 想要淡入的姿势
	FastClip* mClip; // 淡入的动画
	float mTime; // 当前的播放时间
	float mDuration; // 淡入持续时间
	float mElapsed; // 当前淡入了多久

	inline CrossFadeTarget() : mClip(0), mTime(0.0f), mDuration(0.0f), mElapsed(0.0f) {}
	inline CrossFadeTarget(FastClip* target, Pose& pose, float dur) : mClip(target), mTime(target->GetStartTime()), mDuration(dur), mPose(pose), mElapsed(0.0f) {}
};

#endif // !_H_CROSSFADETARGET

