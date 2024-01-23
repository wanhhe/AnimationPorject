#ifndef _H_BLENDING_
#define _H_BLENDING_

#include "Pose.h"

bool isInHierarchy(Pose& pose, unsigned int parent, unsigned int search); // 检查某一个关节是否是另一关节的子关节
void Blend(Pose& output, Pose& a, Pose& b, float t, int root); // 从root开始混合

#endif // !_H_BLENDING_

