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
	// ֻҪ��һ���Ϸ��Ϳ�����
	return mPosition.Size() > 1 || mRotation.Size() > 1 || mScale.Size() > 1;
}

float TransformTrack::GetStartTime() {
	float result = 0.0f;
	bool isSet = false; // �Ƿ��Ѿ��кϷ��Ĺ켣

	if (mPosition.Size() > 1) {
		result = mPosition.GetStartTime();
		isSet = true; // �кϷ���֡
	}
	if (mRotation.Size() > 1) {
		float rotationStart = mRotation.GetStartTime();
		if (rotationStart < result || !isSet) { // ��������ڵĿ�ʼʱ�����û�кϷ�֡����ֵ
			result = rotationStart;
			isSet = true;
		}
	}
	if (mScale.Size() > 1) {
		float scalenStart = mScale.GetStartTime();
		if (scalenStart < result || !isSet) { // ��������ڵĿ�ʼʱ�����û�кϷ�֡����ֵ
			result = scalenStart;
			isSet = true;
		}
	}

	return result;
}

float TransformTrack::GetEndTime() {
	float result = 0.0f;
	bool isSet = false; // �Ƿ��Ѿ��кϷ��Ĺ켣

	if (mPosition.Size() > 1) {
		result = mPosition.GetEndTime();
		isSet = true; // �кϷ���֡
	}
	if (mRotation.Size() > 1) {
		float rotationEnd = mRotation.GetEndTime();
		if (rotationEnd > result || !isSet) { // ��������ڵĿ�ʼʱ�����û�кϷ�֡����ֵ
			result = rotationEnd;
			isSet = true;
		}
	}
	if (mScale.Size() > 1) {
		float scalenEnd = mScale.GetEndTime();
		if (scalenEnd > result || !isSet) { // ��������ڵĿ�ʼʱ�����û�кϷ�֡����ֵ
			result = scalenEnd;
			isSet = true;
		}
	}

	return result;
}

Transform TransformTrack::Sample(const Transform& ref, float time, bool looping) {
	Transform result = ref; // Ĭ�ϵı任
	if (mPosition.Size() > 1) { // �Ϸ��켣�Ž��б任
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