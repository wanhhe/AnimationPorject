#ifndef _H_CROSSFADECONTROLLER_H
#define _H_CROSSFADECONTROLLER_H

#include "CrossFadeTarget.h"
#include "Skeleton.h"
#include <vector>

class CrossFadeController {
protected:
	std::vector<CrossFadeTarget> mTargets; // ���ڵ����Ŀ�궯�������ļ���.
	FastClip* mClip; // ��ǰƬ��
	float mTime; // ��ǰ�Ĳ���ʱ��
	Pose mPose; // ��ǰ����
	Skeleton mSkeleton; // ��ǰ����
	bool mWasSkeletonSet;

public:
	CrossFadeController();
	CrossFadeController(Skeleton& skeleton);
	void SetSkeleton(Skeleton& skeleton);
	void Play(FastClip* target); // ����һ���µĶ���Ƭ�Σ�ͬʱ���֮ǰ�Ķ������䡣
	void FadeTo(FastClip* target, float fadeTime); // ����ǰ�Ķ���������mClip����������һ��Ŀ�������target��
	void Update(float dt);
	Pose& GetCurrentPose();
	FastClip* GetCurrentClip();

};

#endif // !_H_CROSSFADECONTROLLER_H

