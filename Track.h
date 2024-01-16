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
	// 只需要帧信息的集合和插值类型
	std::vector<Frame<N>> mFrames;
	Interpolation mInterpolation;

	T SampleConstant(float time, bool looping);
	T SampleLinear(float time, bool looping);
	T SampleCubic(float time, bool looping);
	T Hermite(float time, const T& point1, const T& slope1, const T& _point2, const T& slope2);
	int FrameIndex(float time, bool looping); // 获得指定时间前的一帧的索引
	float AdjustTimeToFitTrack(float time, bool looping); // 将track范围外的时间调整到track内
	T Cast(float* value); // 用于将Frame中存储的浮点数组转化为对应的类型

public:
	Track();
	void Resize(unsigned int size);
	unsigned int Size();
	Interpolation GetInterpolation();
	void SetInterpolation(Interpolation interpolation);
	float GetStartTime();
	float GetEndTime();
	T Sample(float time, bool looping); // 根据时间和动画是否循环进行采样
	Frame<N>& operator[](unsigned int index);
};

typedef Track<float, 1> ScalarTrack;
typedef Track<vec3, 3> VectorTrack;
typedef Track<quat, 4> QuaternionTrack;

#endif // !_H_TRACK_

