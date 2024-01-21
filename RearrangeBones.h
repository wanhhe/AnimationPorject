#ifndef _H_REARRANGEBONES_
#define _H_REARRANGEBONES_

#include <map>
#include "Skeleton.h"
#include "Clip.h"
#include "Mesh.h"

typedef std::map<int, int> BoneMap;

BoneMap RearrangeSkeleton(Skeleton& skeleton);
void RearrangeMesh(Mesh& mesh, BoneMap& boneMap);
void RearrangeClip(Clip& clip, BoneMap& boneMap);

#endif // !_H_REARRANGEBONES_

