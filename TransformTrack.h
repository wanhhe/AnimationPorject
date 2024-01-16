#ifndef _H_TRANSFORMTRACK_
#define _H_TRANSFORMTRACK_

#include "Track.h"
#include "Transform.h"

class TransformTrack {
protected:
	unsigned int mId; // ��Ӧ��Ҫ�任�Ĺ�����id
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
	float GetStartTime(); // ���غϷ��켣������Ŀ�ʼʱ��
	float GetEndTime(); // ���غϷ��켣������Ŀ�ʼʱ��
	bool IsValid(); // �ж����Ƿ���һ���켣�ǺϷ���
	Transform Sample(const Transform& ref, float time, bool looping);
};


#endif // !_H_TRANSFORMTRACK_

