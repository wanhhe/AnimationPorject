#include "Transform.h"
#include <cmath>

Transform combine(const Transform& a, const Transform& b) {
	Transform out;
	// 从右向左乘，左边的先发挥作用
	out.scale = a.scale * b.scale;
	out.rotation = a.rotation * b.rotation;

	out.position = a.rotation * (a.scale * b.position);
	out.position = a.position + out.position;

	return out;
}

Transform inverse(const Transform& t) {
	Transform inv;
	inv.rotation = inverse(t.rotation);
	inv.scale.x = fabs(t.scale.x) < VEC3_EPSILON ? 0.0f : 1.0f / t.scale.x;
	inv.scale.y = fabs(t.scale.y) < VEC3_EPSILON ? 0.0f : 1.0f / t.scale.y;
	inv.scale.z = fabs(t.scale.z) < VEC3_EPSILON ? 0.0f : 1.0f / t.scale.z;

	vec3 invTrans = t.position * -1.0f;
	inv.position = inv.rotation * (inv.scale * invTrans);

	return inv;
}

Transform mix(const Transform& a, const Transform& b, float t) {
	// 四元数的插值要解决neibourhood问题
	quat bRot = b.rotation;
	if (dot(a.rotation, bRot) < 0.0f) {
		bRot = -bRot;
	}

	return Transform(
		lerp(a.position, b.position, t),
		nlerp(a.rotation, bRot, t),
		lerp(a.scale, b.scale, t)
	);
}

mat4 transformToMat4(const Transform& t) {
	// 先转换坐标系？
	vec3 x = t.rotation * vec3(1, 0, 0);
	vec3 y = t.rotation * vec3(0, 1, 1);
	vec3 z = t.rotation * vec3(0, 0, 1);
	// 不知道先放缩后旋转是什么结果
	x = t.scale * x;
	y = t.scale * y;
	z = t.scale * z;

	vec3 p = t.position;

	return mat4(
		x.x, x.y, x.z, 0,
		y.x, y.y, y.z, 0,
		z.x, z.y, z.z, 0,
		0, 0, 0, 1
	);
}

Transform mat4ToTransform(const mat4& m) {
	Transform out;
	out.position = vec3(m.v[12], m.v[13], m.v[14]); //  矩阵的最后一列是偏移量
	out.rotation = mat4ToQuat(m);

	mat4 rotScaleMat = mat4(
		m.v[0], m.v[1], m.v[2], 0,
		m.v[4], m.v[5], m.v[6], 0,
		m.v[8], m.v[9], m.v[10], 0,
		m.v[12], m.v[13], m.v[14], 0
	);
	// 消去旋转
	mat4 invRotMat = quatToMat4(inverse(out.rotation));
	mat4 scaleSkewMat = rotScaleMat * invRotMat;
	// 对角线是缩放信息
	out.scale = vec3(
		scaleSkewMat.v[0],
		scaleSkewMat.v[5],
		scaleSkewMat.v[10]
	);
	return out;
}

vec3 transformPoint(const Transform& a, const vec3& b) {
	vec3 out;
	out = a.rotation * (a.scale * b);
	out = a.position + out;
	return out;
}

vec3 transformVector(const Transform& a, const vec3& b) {
	vec3 out;
	out = a.rotation * (a.scale * b); //向量不存在偏移，所以不用加position
	return out;
}