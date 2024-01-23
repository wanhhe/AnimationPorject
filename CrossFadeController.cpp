#include "CrossFadeController.h"
#include "Blending.h"

CrossFadeController::CrossFadeController() {
	mClip = 0;
	mTime = 0.0f;
	mWasSkeletonSet = false;
}

CrossFadeController::CrossFadeController(Skeleton& skeleton) {
	mClip = 0;
	mTime = 0.0f;
	SetSkeleton(skeleton);
}

void CrossFadeController::SetSkeleton(Skeleton& skeleton) {
	mSkeleton = skeleton;
	mPose = skeleton.GetRestPose();
	mWasSkeletonSet = true;
}

void CrossFadeController::Play(FastClip* target) {
	mTargets.clear();
	mPose = mSkeleton.GetRestPose();
	mTime = target->GetStartTime();
}

void CrossFadeController::FadeTo(FastClip* target, float fadeTime) {
	if (mClip == 0) { // �����ǰû���κ�Ƭ��,˵����ǰû�в����κζ�������������ֱ�Ӳ��Ŵ����Ŀ�궯��������target����������mTargets�еĵ�һ��Ԫ�أ��������Խ�ʡ����ʱ�䣬��߶�������Ӧ�ٶȡ�
		Play(target); 
		return;
	}
	if (mTargets.size() >= 1) {
		FastClip* clip = mTargets[mTargets.size() - 1].mClip;
		if (clip == target) { // ���Ҫ�����Ŀ�궯���Ѿ�����󣬾Ͳ���Ҫ�����������ε����ˡ����������ε����ʹЧ����ú����
			return;
		}
	}
	else { // ���û�д�����Ƭ��
		if (mClip == target) { // �������������ͬ�������е���
			return;
		}
	}

	mTargets.push_back(CrossFadeTarget(target, mSkeleton.GetRestPose(), fadeTime));
}

void CrossFadeController::Update(float dt) {
	if (mClip == 0 || !mWasSkeletonSet) {
		return;
	}

	unsigned int numTargets = mTargets.size();
	for (unsigned int i = 0; i < numTargets; i++) { // �ҵ��Ѿ�������Ķ�������Ϊ�µı���������
		float duration = mTargets[i].mDuration;
		if (mTargets[i].mElapsed >= duration) { // ����ö���Ƭ�εĲ���ʱ���Ѿ����
			// ���õ��붯������Ϊ������Ķ���
			mClip = mTargets[i].mClip;
			mTime = mTargets[i].mTime;
			mPose = mTargets[i].mPose;
			// ��Ҫ����Ķ���������ɾ��
			mTargets.erase(mTargets.begin() + i);
			break;
		}
	}

	numTargets = mTargets.size();
	mPose = mSkeleton.GetRestPose();
	mTime = mClip->Sample(mPose, mTime + dt);

	for (unsigned int i = 0; i < numTargets; i++) {
		CrossFadeTarget& target = mTargets[i];
		target.mTime = target.mClip->Sample(target.mPose, target.mTime + dt);
		target.mElapsed += dt; // �־�����dtʱ��
		float t = target.mElapsed / target.mDuration;
		if (t > 1.0f) {
			t = 1.0f;
		}
		// ���
		Blend(mPose, mPose, target.mPose, t, -1);
	}
}

Pose& CrossFadeController::GetCurrentPose() {
	return mPose;
}
FastClip* CrossFadeController::GetCurrentClip() {
	return mClip;
}