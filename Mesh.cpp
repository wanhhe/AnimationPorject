#include "Mesh.h"
#include "Draw.h"

Mesh::Mesh() {
	mPosAttrib = new Attribute<vec3>();
	mNormAttrib = new Attribute<vec3>();
	mUvAttrib = new Attribute<vec2>();
	mWeightAttrib = new Attribute<vec4>();
	mInfluenceAttrib = new Attribute<ivec4>();
	mIndexBuffer = new IndexBuffer();
}

Mesh::Mesh(const Mesh& other) {
	mPosAttrib = new Attribute<vec3>();
	mNormAttrib = new Attribute<vec3>();
	mUvAttrib = new Attribute<vec2>();
	mWeightAttrib = new Attribute<vec4>();
	mInfluenceAttrib = new Attribute<ivec4>();
	mIndexBuffer = new IndexBuffer();

	*this = other;
}

Mesh& Mesh::operator=(const Mesh& other) {
	if (this == &other) {
		return *this;
	}

	mPosition = other.mPosition;
	mNormal = other.mNormal;
	mTexCoord = other.mTexCoord;
	mWeights = other.mWeights;
	mInfluences = other.mInfluences;
	mIndices = other.mIndices;
	UpdateOpenGLBuffers(); // 上传到GPU

	return *this;
}
Mesh::~Mesh() {
	delete mPosAttrib;
	delete mNormAttrib;
	delete mUvAttrib;
	delete mWeightAttrib;
	delete mInfluenceAttrib;
	delete mIndexBuffer;
}

std::vector<vec3>& Mesh::GetPosition() {
	return mPosition;
}

std::vector<vec3>& Mesh::GetNormal() {
	return mNormal;
}

std::vector<vec2>& Mesh::GetTexCoord() {
	return mTexCoord;
}

std::vector<vec4>& Mesh::GetWeights() {
	return mWeights;
}

std::vector<ivec4>& Mesh::GetInfluences() {
	return mInfluences;
}

std::vector<unsigned int>& Mesh::GetIndices() {
	return mIndices;
}

void Mesh::UpdateOpenGLBuffers() {
	// 如果有东西就设置
	if (mPosition.size() > 0) {
		mPosAttrib->Set(mPosition);
	}
	if (mNormal.size() > 0) {
		mNormAttrib->Set(mNormal);
	}
	if (mTexCoord.size() > 0) {
		mUvAttrib->Set(mTexCoord);
	}
	if (mWeights.size() > 0) {
		mWeightAttrib->Set(mWeights);
	}
	if (mInfluences.size() > 0) {
		mInfluenceAttrib->Set(mInfluences);
	}
	if (mIndices.size() > 0) {
		mIndexBuffer->Set(mIndices);
	}
}

void Mesh::Bind(int position, int normal, int texcoord, int weight, int influence) {
	if (position >= 0) {
		mPosAttrib->BindTo(position);
	}
	if (normal >= 0) {
		mNormAttrib->BindTo(normal);
	}
	if (texcoord >= 0) {
		mUvAttrib->BindTo(texcoord);
	}
	if (weight >= 0) {
		mWeightAttrib->BindTo(weight);
	}
	if (influence >= 0) {
		mInfluenceAttrib->BindTo(influence);
	}
}

void Mesh::Draw() {
	if (mIndices.size() > 0) {
		::Draw(*mIndexBuffer, DrawMode::Triangles);
	}
	else { // 不够绘制三角形或是不采用ebo
		::Draw(mPosition.size(), DrawMode::Triangles);
	}
}

void Mesh::DrawInstanced(unsigned int numInstances) {
	if (mIndices.size() > 0) {
		::DrawInstance(*mIndexBuffer, DrawMode::Triangles, numInstances);
	}
	else {
		::DrawInstance(mPosition.size(), DrawMode::Triangles, numInstances);
	}
}

void Mesh::UnBind(int position, int normal, int texcoord, int weight, int influence) {
	if (position >= 0) {
		mPosAttrib->UnBindFrom(position);
	}
	if (normal >= 0) {
		mNormAttrib->UnBindFrom(normal);
	}
	if (texcoord >= 0) {
		mUvAttrib->UnBindFrom(texcoord);
	}
	if (weight >= 0) {
		mWeightAttrib->UnBindFrom(weight);
	}
	if (influence >= 0) {
		mInfluenceAttrib->UnBindFrom(influence);
	}
}

#if 1
void Mesh::CPUSkin(Skeleton& skeleton, Pose& pose) {
	unsigned int numVerts = (unsigned int)mPosition.size();
	if (numVerts == 0) return;
	mSkinnedPosition.resize(numVerts);
	mSkinnedNormal.resize(numVerts);

	pose.GetMatrixPalette(mPosePalette); // 获得Transform对应的矩阵形式
	std::vector<mat4> invPosePalette = skeleton.GetInvBindPose(); // 获得每个骨骼关节对应的绑定姿势逆矩阵

	for (unsigned int i = 0; i < numVerts; i++) {
		ivec4& joint = mInfluences[i];
		vec4& weight = mWeights[i];

		mat4 m0 = (mPosePalette[joint.x] * invPosePalette[joint.x]) * weight.x;
		mat4 m1 = (mPosePalette[joint.y] * invPosePalette[joint.y]) * weight.y;
		mat4 m2 = (mPosePalette[joint.z] * invPosePalette[joint.z]) * weight.z;
		mat4 m3 = (mPosePalette[joint.w] * invPosePalette[joint.w]) * weight.w;

		mat4 skin = m0 + m1 + m2 + m3;
		mSkinnedPosition[i] = transformPoint(skin, mPosition[i]);
		mSkinnedNormal[i] = transformVector(skin, mNormal[i]);
	}

	mPosAttrib->Set(mSkinnedPosition);
	mNormAttrib->Set(mSkinnedNormal);
}
#else
void Mesh::CPUSkin(Skeleton& skeleton, Pose& pose) {
	unsigned int numVerts = (unsigned int)mPosition.size();
	if (numVerts == 0) return;
	mSkinnedPosition.resize(numVerts);
	mSkinnedNormal.resize(numVerts);
	Pose& bindPose = skeleton.GetBindPose();
	for (unsigned int i = 0; i < numVerts; i++) {
		ivec4& joint = mInfluences[i]; // 对该顶点产生影响的骨骼索引
		vec4& weight = mWeights[i]; // 每个骨骼索引的权重

		// 第一个骨骼关节的影响
		Transform skin0 = combine(pose[joint.x], inverse(bindPose[joint.x])); // 如果想要从模型空间变换到骨骼空间，那么首先要乘以绑定姿势逆矩阵，然后再乘以当前姿势矩阵。
		vec3 p0 = transformPoint(skin0, mPosition[i]);
		vec3 n0 = transformVector(skin0, mNormal[i]);
		// 第二个骨骼关节的影响
		Transform skin1 = combine(pose[joint.y], inverse(bindPose[joint.y]));
		vec3 p1 = transformPoint(skin1, mPosition[i]);
		vec3 n1 = transformVector(skin1, mNormal[i]);
		// 第三个骨骼关节的影响
		Transform skin2 = combine(pose[joint.z], inverse(bindPose[joint.z]));
		vec3 p2 = transformPoint(skin2, mPosition[i]);
		vec3 n2 = transformVector(skin2, mNormal[i]);
		// 第四个骨骼关节的影响
		Transform skin3 = combine(pose[joint.w], inverse(bindPose[joint.w]));
		vec3 p3 = transformPoint(skin3, mPosition[i]);
		vec3 n3 = transformVector(skin3, mNormal[i]);

		// blend
		mSkinnedPosition[i] = p0 * weight.x + p1 * weight.y + p2 * weight.z + p3 * weight.w;
		mSkinnedNormal[i] = n0 * weight.x + n1 * weight.y + n2 * weight.z + n3 * weight.w;
	}

	mPosAttrib->Set(mSkinnedPosition);
	mNormAttrib->Set(mSkinnedNormal);
}
#endif

void Mesh::CPUSkin(std::vector<mat4>& animatedPose) {
	unsigned int numVerts = (unsigned int)mPosition.size();
	if (numVerts == 0) return;

	mSkinnedPosition.resize(numVerts);
	mSkinnedNormal.resize(numVerts);

	for (unsigned int i = 0; i < numVerts; i++) { // 对每个顶点计算骨骼对其的影响
		vec4& weights = mWeights[i];
		ivec4& joints = mInfluences[i];

		// 由于是已经计算好的矩阵，直接组合即可
		vec3 p0 = transformPoint(animatedPose[joints.x], mPosition[i]);
		vec3 p1 = transformPoint(animatedPose[joints.y], mPosition[i]);
		vec3 p2 = transformPoint(animatedPose[joints.z], mPosition[i]);
		vec3 p3 = transformPoint(animatedPose[joints.w], mPosition[i]);
		mSkinnedPosition[i] = p0 * weights.x + p1 * weights.y + p2 * weights.z + p3 * weights.w; // 加权
		
		// 法线同理
		vec3 n0 = transformVector(animatedPose[joints.x], mNormal[i]);
		vec3 n1 = transformVector(animatedPose[joints.y], mNormal[i]);
		vec3 n2 = transformVector(animatedPose[joints.z], mNormal[i]);
		vec3 n3 = transformVector(animatedPose[joints.w], mNormal[i]);
		mSkinnedNormal[i] = n0 * weights.x + n1 * weights.y + n2 * weights.z + n3 * weights.w; // 加权
	}

	mPosAttrib->Set(mSkinnedPosition);
	mNormAttrib->Set(mSkinnedNormal);
}