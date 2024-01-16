#include "TransformTrack.h"

TransformTrack::TransformTrack() {
	mId = 0;
}

unsigned int TransformTrack::GetId() {
	return mId;
}

void TransformTrack::SetId(unsigned int id) {
	mId = id;
}

VectorTrack& TransformTrack::GetPositionTrack() {
	return mPosition;
}

QuaternionTrack& TransformTrack::GetRotationTrack() {
	return mRotation;
}

VectorTrack& TransformTrack::GetScaleTrack() {
	return mScale;
}

bool TransformTrack::IsValid() {
	// 只要有一个合法就可以了
	return mPosition.Size() > 1 || mRotation.Size() > 1 || mScale.Size() > 1;
}

float TransformTrack::GetStartTime() {
	float result = 0.0f;
	bool isSet = false; // 是否已经有合法的轨迹

	if (mPosition.Size() > 1) {
		result = mPosition.GetStartTime();
		isSet = true; // 有合法的帧
	}
	if (mRotation.Size() > 1) {
		float rotationStart = mRotation.GetStartTime();
		if (rotationStart < result || !isSet) { // 如果比现在的开始时间早或还没有合法帧，则赋值
			result = rotationStart;
			isSet = true;
		}
	}
	if (mScale.Size() > 1) {
		float scalenStart = mScale.GetStartTime();
		if (scalenStart < result || !isSet) { // 如果比现在的开始时间早或还没有合法帧，则赋值
			result = scalenStart;
			isSet = true;
		}
	}

	return result;
}

float TransformTrack::GetEndTime() {
	float result = 0.0f;
	bool isSet = false; // 是否已经有合法的轨迹

	if (mPosition.Size() > 1) {
		result = mPosition.GetEndTime();
		isSet = true; // 有合法的帧
	}
	if (mRotation.Size() > 1) {
		float rotationEnd = mRotation.GetEndTime();
		if (rotationEnd > result || !isSet) { // 如果比现在的开始时间早或还没有合法帧，则赋值
			result = rotationEnd;
			isSet = true;
		}
	}
	if (mScale.Size() > 1) {
		float scalenEnd = mScale.GetEndTime();
		if (scalenEnd > result || !isSet) { // 如果比现在的开始时间早或还没有合法帧，则赋值
			result = scalenEnd;
			isSet = true;
		}
	}

	return result;
}

Transform TransformTrack::Sample(const Transform& ref, float time, bool looping) {
	Transform result = ref; // 默认的变换
	if (mPosition.Size() > 1) { // 合法轨迹才进行变换
		result.position = mPosition.Sample(time, looping);
	}
	if (mRotation.Size() > 1) {
		result.rotation = mRotation.Sample(time, looping);
	}
	if (mScale.Size() > 1) {
		result.scale = mScale.Sample(time, looping);
	}
	
	return result;
}