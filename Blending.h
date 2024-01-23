#ifndef _H_BLENDING_
#define _H_BLENDING_

#include "Pose.h"

bool isInHierarchy(Pose& pose, unsigned int parent, unsigned int search); // ���ĳһ���ؽ��Ƿ�����һ�ؽڵ��ӹؽ�
void Blend(Pose& output, Pose& a, Pose& b, float t, int root); // ��root��ʼ���

#endif // !_H_BLENDING_

