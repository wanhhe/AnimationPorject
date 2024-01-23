#ifndef _H_CROSSFADETARGET
#define _H_CROSSFADETARGET

#include "Pose.h"
#include "Clip.h"

struct CrossFadeTarget
{
	Pose mPose; // ��Ҫ���������
	FastClip* mClip; // ����Ķ���
	float mTime; // ��ǰ�Ĳ���ʱ��
	float mDuration; // �������ʱ��
	float mElapsed; // ��ǰ�����˶��

	inline CrossFadeTarget() : mClip(0), mTime(0.0f), mDuration(0.0f), mElapsed(0.0f) {}
	inline CrossFadeTarget(FastClip* target, Pose& pose, float dur) : mClip(target), mTime(target->GetStartTime()), mDuration(dur), mPose(pose), mElapsed(0.0f) {}
};

#endif // !_H_CROSSFADETARGET

