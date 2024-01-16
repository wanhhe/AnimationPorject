#ifndef _H_TRACK_
#define _H_TRACK_

#include <vector>
#include "Frame.h"
#include "Interpolation.h"
#include "vec3.h"
#include "quat.h"

template<typename T, int N>
class Track {
protected:
	// ֻ��Ҫ֡��Ϣ�ļ��ϺͲ�ֵ����
	std::vector<Frame<N>> mFrames;
	Interpolation mInterpolation;

	T SampleConstant(float time, bool looping);
	T SampleLinear(float time, bool looping);
	T SampleCubic(float time, bool looping);
	T Hermite(float time, const T& point1, const T& slope1, const T& _point2, const T& slope2);
	int FrameIndex(float time, bool looping); // ���ָ��ʱ��ǰ��һ֡������
	float AdjustTimeToFitTrack(float time, bool looping); // ��track��Χ���ʱ�������track��
	T Cast(float* value); // ���ڽ�Frame�д洢�ĸ�������ת��Ϊ��Ӧ������

public:
	Track();
	void Resize(unsigned int size);
	unsigned int Size();
	Interpolation GetInterpolation();
	void SetInterpolation(Interpolation interpolation);
	float GetStartTime();
	float GetEndTime();
	T Sample(float time, bool looping); // ����ʱ��Ͷ����Ƿ�ѭ�����в���
	Frame<N>& operator[](unsigned int index);
};

typedef Track<float, 1> ScalarTrack;
typedef Track<vec3, 3> VectorTrack;
typedef Track<quat, 4> QuaternionTrack;

#endif // !_H_TRACK_

