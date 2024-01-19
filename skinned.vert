#version 330 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

in vec3 position;
in vec3 normal;
in vec2 texCoord;
in vec4 weights;
in ivec4 joints;

uniform mat4 pose[120];
uniform mat4 invBindPose[120];
out vec3 norm;
out vec3 fragPos; // 世界坐标中的位置
out vec2 uv;

void main() {
	mat4 skin = (pose[joints.x] * invBindPose[joints.x]) * weight.x;
	skin += (pose[joints.y] * invBindPose[joints.y]) * weight.y;
	skin += (pose[joints.z] * invBindPose[joints.z]) * weight.z;
	skin += (pose[joints.w] * invBindPose[joints.w]) * weight.w;
	gl_Position = projection * view * model * skin * vec4(position, 1.0);
	fragPos = vec3(model * skin * vec4(position, 1.0f));
	norm = vec3(model * skin * vec4(normal, 0.0f));
	uv = texCoord;
}