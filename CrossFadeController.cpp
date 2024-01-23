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
	if (mClip == 0) { // 如果当前没有任何片段,说明当前没有播放任何动画剪辑，所以直接播放传入的目标动画剪辑（target），而不是mTargets中的第一个元素，这样可以节省淡入时间，提高动画的响应速度。
		Play(target); 
		return;
	}
	if (mTargets.size() >= 1) {
		FastClip* clip = mTargets[mTargets.size() - 1].mClip;
		if (clip == target) { // 如果要淡入的目标动画已经在最后，就不需要连续进行两次淡入了。连续的两次淡入会使效果变得很糟糕
			return;
		}
	}
	else { // 如果没有待淡出片段
		if (mClip == target) { // 如果两个动画相同，不进行淡出
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
	for (unsigned int i = 0; i < numTargets; i++) { // 找到已经播放完的动画设置为新的被淡出动画
		float duration = mTargets[i].mDuration;
		if (mTargets[i].mElapsed >= duration) { // 如果该动画片段的播放时间已经完毕
			// 将该淡入动画设置为被淡入的动画
			mClip = mTargets[i].mClip;
			mTime = mTargets[i].mTime;
			mPose = mTargets[i].mPose;
			// 从要淡入的动画队列中删除
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
		target.mElapsed += dt; // 又经过了dt时间
		float t = target.mElapsed / target.mDuration;
		if (t > 1.0f) {
			t = 1.0f;
		}
		// 混合
		Blend(mPose, mPose, target.mPose, t, -1);
	}
}

Pose& CrossFadeController::GetCurrentPose() {
	return mPose;
}
FastClip* CrossFadeController::GetCurrentClip() {
	return mClip;
}