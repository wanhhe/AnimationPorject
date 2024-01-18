#ifndef _H_CLIP_
#define _H_CLIP_

#include "TransformTrack.h"
#include <vector>
#include <string>
#include "Pose.h"

class Clip {
protected:
	std::vector<TransformTrack> mTracks;
	std::string mName;
	float mStartTime;
	float mEndTime;
	bool mLooping;

	float AdjustTimeToFitRange(float time);

public:
	Clip();

	unsigned int GetIdAtIndex(unsigned int index);
	void SetIdAtIndex(unsigned int index, unsigned int id);
	unsigned int Size();
	float Sample(Pose& outPose, float time); // ��Clip���������������һ��Pose
	TransformTrack& operator[](unsigned int joint); // ���ݹ���id���
	void RecalculateDuration(); // ����Clip��ʱ����

	std::string& GetName();
	void SetName(const std::string& newName);
	float GetStartTime();
	float GetEndTime();
	float GetDuration();
	bool GetLooping();
	void SetLooping(bool looping);
};

#endif // !_H_CLIP_

