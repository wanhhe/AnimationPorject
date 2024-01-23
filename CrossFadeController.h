#ifndef _H_CROSSFADECONTROLLER_H
#define _H_CROSSFADECONTROLLER_H

#include "CrossFadeTarget.h"
#include "Skeleton.h"
#include <vector>

class CrossFadeController {
protected:
	std::vector<CrossFadeTarget> mTargets; // 正在淡入的目标动画剪辑的集合.
	FastClip* mClip; // 当前片段
	float mTime; // 当前的播放时间
	Pose mPose; // 当前姿势
	Skeleton mSkeleton; // 当前骨骼
	bool mWasSkeletonSet;

public:
	CrossFadeController();
	CrossFadeController(Skeleton& skeleton);
	void SetSkeleton(Skeleton& skeleton);
	void Play(FastClip* target); // 播放一个新的动画片段，同时清除之前的动画渐变。
	void FadeTo(FastClip* target, float fadeTime); // 将当前的动画剪辑（mClip）淡出到另一个目标剪辑（target）
	void Update(float dt);
	Pose& GetCurrentPose();
	FastClip* GetCurrentClip();

};

#endif // !_H_CROSSFADECONTROLLER_H

