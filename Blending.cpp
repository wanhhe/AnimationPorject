#include "Blending.h"

bool isInHierarchy(Pose& pose, unsigned int parent, unsigned int search) {
	if (search == parent) return true;

	int p = pose.GetParent(search);
	while (p >= 0)
	{
		if (p == (int)parent) {
			return true;
		}
		p = pose.GetParent(p);
	}

	return false;
}

void Blend(Pose& output, Pose& a, Pose& b, float t, int root) {
	unsigned int numJoints = output.Size();
	for (unsigned int i = 0; i < numJoints; i++) {
		if (root >= 0) { // 如果不是对整个动画进行混合
			if (!isInHierarchy(output, root, i)) {
				continue; // 如果不是该关节的子关节，就不进行混合
			}
		}

		output.SetLocalTransform(i, mix(a.GetLocalTransform(i), b.GetLocalTransform(i), t));
	}
}