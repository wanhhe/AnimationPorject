#include "GLTFLoder.h"
#include <iostream>
#include "Transform.h"

namespace GLTFHelpers {
	// 读取某个结点存储的变换
	Transform GetLocalTransform(cgltf_node& node) {
		Transform result;

		if (node.has_matrix) {
			mat4 mat(&node.matrix[0]);
			result = mat4ToTransform(mat);
		}

		if (node.has_translation) {
			result.position = vec3(node.translation[0], node.translation[1], node.translation[2]);
		}
		if (node.has_rotation) {
			result.rotation = quat(node.rotation[0], node.rotation[1], node.rotation[2], node.rotation[3]);
		}
		if (node.has_scale) {
			result.scale = vec3(node.scale[0], node.scale[1], node.scale[2]);
		}

		return result;
	}

	int GetNodeIndex(cgltf_node* target, cgltf_node* allNodes, unsigned int numNodes) {
		if (target == 0) { // 0表示空指针，非法指针不进行查找
			return -1;
		}

		for (unsigned int i = 0; i < numNodes; i++) {
			if (&allNodes[i] == target) return (int)i;
		}

		return -1; // 没有找到
	}

	// compCount: 一个结点由几个float值组成
	void GetScalarValues(std::vector<float>& out, unsigned int compCount, const cgltf_accessor& inAccessor) {
		out.resize(inAccessor.count * compCount); // inAccessor.count表示数据元素的个数，如vec3的个数，quat的个数。每个数据元素可表示value、in、out等
		for (cgltf_size i = 0; i < inAccessor.count; ++i) {
			cgltf_accessor_read_float(&inAccessor, i, &out[i * compCount], compCount);
		}
	}

	template<typename T, int N>
	void TrackFromChannel(Track<T, N>& inOutTrack, const cgltf_animation_channel& inChannel) {
		cgltf_animation_sampler& sampler = *inChannel.sampler;

		// 设置插值类型
		Interpolation interpolation = Interpolation::Constant;
		if (inChannel.sampler->interpolation == cgltf_interpolation_type_linear) {
			interpolation = Interpolation::Linear;
		}
		else if (inChannel.sampler->interpolation == cgltf_interpolation_type_cubic_spline) {
			interpolation = Interpolation::Cubic;
		}
		bool isSamplerCubic = interpolation == Interpolation::Cubic; // 用于确定后面是否设置切线数据
		inOutTrack.SetInterpolation(interpolation);

		std::vector<float> timelineFloats;
		GetScalarValues(timelineFloats, 1, *sampler.input); // sampler的输入是对时间轴的访问器

		std::vector<float> valueFloats;
		GetScalarValues(valueFloats, N, *sampler.output); // sampler的输出是根据和时间轴对应的值，可以是旋转、位移、缩放

		unsigned int numFrames = (unsigned int)sampler.input->count; // 采样时间轴的个数就是帧数
		unsigned int numberOfPerFrame = valueFloats.size() / timelineFloats.size(); // 每个数据元素的个数，timelineFloats.size()应该也可以换成帧数
		inOutTrack.Resize(numFrames);
		// 读取每帧的数据
		for (unsigned int i = 0; i < numFrames; i++) {
			int baseIndex = i * numberOfPerFrame;
			Frame& frame = inOutTrack[i];
			int offset = 0;

			frame.mTime = timelineFloats[i];
			for (int component = 0; component < N; component++) {
				frame.mIn[component] = isSamplerCubic ? valueFloats[baseIndex + offset++] : 0.0f;
			}
			for (int component = 0; component < N; component++) {
				frame.mValue[component] = valueFloats[baseIndex + offset++];
			}
			for (int component = 0; component < N; component++) {
				frame.mOut[component] = isSamplerCubic ? valueFloats[baseIndex + offset++] : 0.0f;
			}
		}
	}
}

cgltf_data* LoadGLTFFile(const char* path) {
	cgltf_options options;
	memset(&options, 0, sizeof(cgltf_options)); // 先将结构体内的元素全填充为0

	cgltf_data* data = NULL;
	cgltf_result result = cgltf_parse_file(&options, path, &data);
	if (result != cgltf_result_success) {
		std::cout << "Could not load input file: " << path << "\n";
		return 0;
	}
	result = cgltf_load_buffers(&options, data, path);
	if (result != cgltf_result_success) {
		cgltf_free(data);
		std::cout << "Could not load buffer for: " << path << "\n";
		return 0;
	}
	result = cgltf_validate(data);
	if (result != cgltf_result_success) {
		cgltf_free(data);
		std::cout << "Invalid gltf file: " << path << "\n";
		return 0;
	}
	return data;
}

void FreeGLTFFile(cgltf_data* handle) {
	if (handle == 0) {
		std::cout << "WARNING: Can't free null data\n";
	}
	else {
		cgltf_free(handle);
	}
}

Pose LoadRestPose(cgltf_data* data) {
	unsigned int boneCount = data->nodes_count;
	Pose result(boneCount);

	for (unsigned int i = 0; i < boneCount; i++) {
		cgltf_node* node = &(data->nodes[i]);

		// 设置变换
		Transform transform = GLTFHelpers::GetLocalTransform(data->nodes[i]);
		result.SetLocalTransform(i, transform);

		// 设置其父节点
		int parent = GLTFHelpers::GetNodeIndex(node->parent, data->nodes, boneCount);
		result.SetParent(i, parent);
	}

	return result;
}

std::vector<std::string> LoadJointNames(cgltf_data* data) {
	unsigned int boneCount = (unsigned int)data->nodes_count;
	std::vector<std::string> result(boneCount, "Not Set");

	for (unsigned int i = 0; i < boneCount; i++) {
		cgltf_node* node = &(data->nodes[i]);

		if (node->name == 0) { // 没有名字，指针为空
			result[i] = "EMPTY NODE";
		}
		else {
			result[i] = node->name;
		}
	}

	return result;
}

std::vector<Clip> LoadAnimationClips(cgltf_data* data) {
	unsigned int numClips = (unsigned int)data->animations_count;
	unsigned int numNodes = (unsigned int)data->nodes_count; // 动画中结点的个数

	std::vector<Clip> result;
	result.resize(numClips);

	for (unsigned int i = 0; i < numClips; i++) {
		// 为每个片段设置名称
		result[i].SetName(data->animations[i].name);

		unsigned int numChannels = (unsigned int)data->animations[i].channels_count;
		for (unsigned int j = 0; j < numChannels; j++) { // 通道定义了动画的目标节点和属性（如平移、旋转或缩放)
			cgltf_animation_channel& channel = data->animations[i].channels[j];
			cgltf_node* target = channel.target_node;
			int nodeId = GLTFHelpers::GetNodeIndex(target, data->nodes, numNodes);
			// 根据类型决定采用TransformTrack中的哪个成员
			if (channel.target_path == cgltf_animation_path_type_translation) {
				VectorTrack& track = result[i][nodeId].GetPositionTrack();
				GLTFHelpers::TrackFromChannel<vec3, 3>(track, channel);
			}
			else if (channel.target_path == cgltf_animation_path_type_scale) {
				VectorTrack& track = result[i][nodeId].GetScaleTrack();
				GLTFHelpers::TrackFromChannel<vec3, 3>(track, channel);
			}
			else if (channel.target_path == cgltf_animation_path_type_rotation) {
				QuaternionTrack& track = result[i][nodeId].GetRotationTrack();
				GLTFHelpers::TrackFromChannel<quat, 4>(track, channel);
			}
		}

		result[i].RecalculateDuration();
	}

	return result;
}