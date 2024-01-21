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
	std::vector<vec2> mTexCoord; // ָ��ÿ�����Ӧ����������
	std::vector<vec4> mWeights; // �����ù⻬��Ƥ�����¼ÿ�������ؽڶ�ĳ�������Ȩ��
	std::vector<ivec4> mInfluences; // �����ù⻬��Ƥ�����¼ÿ�������ؽڵı任�������� contains the joint IDs that affect the vertex
	std::vector<unsigned int> mIndices;

	Attribute<vec3>* mPosAttrib;
	Attribute<vec3>* mNormAttrib;
	Attribute<vec2>* mUvAttrib;
	Attribute<vec4>* mWeightAttrib;
	Attribute<ivec4>* mInfluenceAttrib;
	IndexBuffer* mIndexBuffer;

	// ����CPU��Ƥ
	std::vector<vec3> mSkinnedPosition; // �����洢������Ƥ��Ķ���λ��
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
	void CPUSkin(std::vector<mat4>& animatedPose); // ����������õľ��������Ƥ �Ѿ���pose*invBind��
	void UpdateOpenGLBuffers();

	void Bind(int position, int normal, int texcoord, int weight, int influence); // �󶨵�ָ����slot
	void Draw();
	void DrawInstanced(unsigned int numInstances);
	void UnBind(int position, int normal, int texcoord, int weight, int influence);
};

#endif // !_H_MESH_

