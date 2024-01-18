#include "Clip.h"

Clip::Clip() {
	mName = "default";
	mStartTime = 0.0f;
	mEndTime = 0.0f;
	mLooping = true;
}

float Clip::Sample(Pose& outPose, float time) {
	// 首先判断该Clip是否合法
	if (GetDuration() <= 0.0f) return 0.0f;
	// 矫正时间
	time = AdjustTimeToFitRange(time);

	unsigned int size = mTracks.size();
	for (unsigned int i = 0; i < size; i++) {
		unsigned int joint = mTracks[i].GetId();
		// 如果变换的组件没有设置动画，则使用引用的Pose的默认值
		Transform local = outPose.GetLocalTransform(i);
		Transform animated = mTracks[i].Sample(local, time, mLooping);
		outPose.SetLocalTransform(joint, animated);
	}
	return time;
}

void Clip::RecalculateDuration() {
	mStartTime = 0.0f;
	mEndTime = 0.0f;
	bool startSet = false;
	bool endSet = false;
	unsigned int size = mTracks.size();

	for (unsigned int i = 0; i < size; i++) {
		if (mTracks[i].IsValid()) {
			float startTime = mTracks[i].GetStartTime();
			float endTime = mTracks[i].GetEndTime();
			if (startTime < mStartTime || !startSet) {
				mStartTime = startTime;
				startSet = true;
			}
			if (endTime > mEndTime || !endSet) {
				mEndTime = endTime;
				endSet = true;
			}
		}
	}
}

float Clip::AdjustTimeToFitRange(float time) {
	if (mLooping) {
		float duration = mEndTime - mStartTime;
		if (duration <= 0.0f) return 0.0f;
		time = fmodf(time - mStartTime, duration);
		if (time <= 0.0f) {
			time += duration;
		}
		time += mStartTime;
	}
	else
	{
		if (time < mStartTime) time = mStartTime;
		if (time > mEndTime) time = mEndTime;
	}
	return time;
}

TransformTrack& Clip::operator[](unsigned int joint) {
	unsigned int size = mTracks.size();
	for (unsigned int i = 0; i < size; i++) {
		if (mTracks[i].GetId() == joint) {
			return mTracks[i];
		}
	}

	// 如果没有该轨迹，创建一个并返回
	mTracks.push_back(TransformTrack());
	mTracks[mTracks.size() - 1].SetId(joint);
	return mTracks[mTracks.size() - 1];
}

unsigned int Clip::GetIdAtIndex(unsigned int index) {
	return mTracks[index].GetId();
}

void Clip::setIdAtIndex(unsigned int index, unsigned int id) {
	mTracks[index].SetId(id);
}

unsigned int Clip::Size() {
	return (unsigned int)mTracks.size();
}

std::string& Clip::GetName() {
	return mName;
}

void Clip::SetName(const std::string& newName) {
	mName = newName;
}

float Clip::GetStartTime() {
	return mStartTime;
}

float Clip::GetEndTime() {
	return mEndTime;
}

float Clip::GetDuration() {
	return mEndTime - mStartTime;
}

bool Clip::GetLooping() {
	return mLooping;
}

void Clip::SetLooping(bool looping) {
	mLooping = looping;
}