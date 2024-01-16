#ifndef _H_TRANSFORMTRACK_
#define _H_TRANSFORMTRACK_

#include "Track.h"
#include "Transform.h"

class TransformTrack {
protected:
	unsigned int mId; // 对应的要变换的骨骼的id
	VectorTrack mPosition;
	QuaternionTrack mRotation;
	VectorTrack mScale;

public:
	TransformTrack();
	unsigned int GetId();
	void SetId(unsigned int id);
	VectorTrack& GetPositionTrack();
	QuaternionTrack& GetRotationTrack();
	VectorTrack& GetScaleTrack();
	float GetStartTime(); // 返回合法轨迹中最早的开始时间
	float GetEndTime(); // 返回合法轨迹中最晚的开始时间
	bool IsValid(); // 判断是是否有一个轨迹是合法的
	Transform Sample(const Transform& ref, float time, bool looping);
};


#endif // !_H_TRANSFORMTRACK_

