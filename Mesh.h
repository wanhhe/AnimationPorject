#ifndef _H_MESH_
#define _H_MESH_

#include <vector>
#include "vec3.h"
#include "vec2.h"
#include "vec4.h"
#include "Attribute.h"
#include "IndexBuffer.h"
#include "mat4.h"
#include "Skeleton.h"

class Mesh {
protected:
	std::vector<vec3> mPosition;
	std::vector<vec3> mNormal;
	std::vector<vec2> mTexCoord; // 指定每个点对应的纹理坐标
	std::vector<vec4> mWeights; // 若采用光滑蒙皮，则记录每个骨骼关节对某个顶点的权重
	std::vector<ivec4> mInfluences; // 若采用光滑蒙皮，则记录每个骨骼关节的变换矩阵索引 contains the joint IDs that affect the vertex
	std::vector<unsigned int> mIndices;

	Attribute<vec3>* mPosAttrib;
	Attribute<vec3>* mNormAttrib;
	Attribute<vec2>* mUvAttrib;
	Attribute<vec4>* mWeightAttrib;
	Attribute<ivec4>* mInfluenceAttrib;
	IndexBuffer* mIndexBuffer;

	// 用于CPU蒙皮
	std::vector<vec3> mSkinnedPosition; // 用来存储骨骼蒙皮后的顶点位置
	std::vector<vec3> mSkinnedNormal;
	std::vector<mat4> mPosePalette;

public:
	Mesh();
	Mesh(const Mesh& other);
	Mesh& operator=(const Mesh& other);
	~Mesh();

	std::vector<vec3>& GetPosition();
	std::vector<vec3>& GetNormal();
	std::vector<vec2>& GetTexCoord();
	std::vector<vec4>& GetWeights();
	std::vector<ivec4>& GetInfluences();
	std::vector<unsigned int>& GetIndices();

	void CPUSkin(Skeleton& skeleton, Pose& pose);
	void CPUSkin(std::vector<mat4>& animatedPose); // 传过来计算好的矩阵进行蒙皮 已经是pose*invBind了
	void UpdateOpenGLBuffers();

	void Bind(int position, int normal, int texcoord, int weight, int influence); // 绑定到指定的slot
	void Draw();
	void DrawInstanced(unsigned int numInstances);
	void UnBind(int position, int normal, int texcoord, int weight, int influence);
};

#endif // !_H_MESH_

