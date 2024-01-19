#include "GLTFLoader.h"
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
			Frame<N>& frame = inOutTrack[i];
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

	// 构建Mesh
	void MeshFromAttribute(Mesh& outMesh, cgltf_attribute& attribute, cgltf_skin* skin, cgltf_node* nodes, unsigned int nodeCount) {
		// 该attribute的类型(pos, norm, uv, weight, influence)
		cgltf_attribute_type attribType = attribute.type;
		// attribute的访问器
		cgltf_accessor& accessor = *attribute.data;

		unsigned int componentCount = 0; // 分量是几个
		if (accessor.type == cgltf_type_vec2) {
			componentCount = 2;
		}
		else if (accessor.type == cgltf_type_vec3) {
			componentCount = 3;
		}
		else if (accessor.type == cgltf_type_vec4) {
			componentCount = 4;
		}
		std::vector<float> values; // 用来读取值
		GetScalarValues(values, componentCount, accessor);
		unsigned int accessorCount = (unsigned int)accessor.count; // 数据元素个数

		std::vector<vec3>& positions = outMesh.GetPosition();
		std::vector<vec3>& normals = outMesh.GetNormal();
		std::vector<vec2>& texCoords = outMesh.GetTexCoord();
		std::vector<ivec4>& influences = outMesh.GetInfluences();
		std::vector<vec4>& weights = outMesh.GetWeights();

		for (unsigned int i = 0; i < accessorCount; i++) {
			int index = i * componentCount;
			switch (attribType) {
			case cgltf_attribute_type_position:
				positions.push_back(vec3(values[index], values[index + 1], values[index + 2]));
				break;
			// 当前顶点的uv
			case cgltf_attribute_type_texcoord:
				texCoords.push_back(vec2(values[index], values[index + 1]));
				break;
			// 影响当前顶点的权重
			case cgltf_attribute_type_weights:
				weights.push_back(vec4(values[index], values[index + 1], values[index + 2], values[index + 3]));
				break;
			// 当前顶点的法线
			case cgltf_attribute_type_normal:
			{
				vec3 normal = vec3(values[index + 0], values[index + 1], values[index + 2]);
				if (lenSq(normal) < 0.000001f) {
					normal = vec3(0, 1, 0);
				}
				normals.push_back(normalized(normal));
			}
			break;
			// 影响当前顶点的骨骼的索引
			case cgltf_attribute_type_joints:
			{
				// values是顶点的关节索引
				ivec4 joints((int)(values[index] + 0.5f), (int)(values[index + 1] + 0.5f), (int)(values[index + 2] + 0.5f), (int)(values[index + 3] + 0.5f));
				// 将关节相对于皮肤的索引转换为节点的索引。为了计算关节矩阵，需要知道关节在节点树中的位置和方向，而不是在骨架中的顺序
				joints.x = std::max(0, GetNodeIndex(skin->joints[joints.x], nodes, nodeCount));
				joints.y = std::max(0, GetNodeIndex(skin->joints[joints.y], nodes, nodeCount));
				joints.z = std::max(0, GetNodeIndex(skin->joints[joints.z], nodes, nodeCount));
				joints.w = std::max(0, GetNodeIndex(skin->joints[joints.w], nodes, nodeCount));

				influences.push_back(joints);
			}
			break;
			default:
				break;
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

		// 设置骨骼的变换
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

Pose LoadBindPose(cgltf_data* data) {
	// 先获得restPose作为默认值，避免该文件未设置bindPose而导致错误
	Pose restPose = LoadRestPose(data);
	unsigned int numBones = restPose.Size();
	std::vector<Transform> worldBindPose(numBones);
	for (unsigned int i = 0; i < numBones; i++) {
		worldBindPose[i] = restPose.GetGlobalTransform(i); // 设置每个关节的默认值
	}

	unsigned int numSkins = data->skins_count;
	for (unsigned int i = 0; i < numSkins; i++) { // 获得每个皮肤的逆绑定矩阵(全局矩阵)
		cgltf_skin* skin = &(data->skins[i]);
		std::vector<float> invBindAccessor;
		GLTFHelpers::GetScalarValues(invBindAccessor, 16, *skin->inverse_bind_matrices); // 一个元素由一个4*4的矩阵构成
	
		// 获得该蒙皮上每个关节的逆绑定矩阵
		unsigned int numJoints = skin->joints_count;
		for (unsigned int j = 0; j < numJoints; j++) {
			// 获得该关节的 inverse bind matrix
			float* matrix = &invBindAccessor[j * 16];
			mat4 invBindMatrix = mat4(matrix);
			// 获得绑定矩阵
			mat4 bindMatrix = inverse(invBindMatrix);
			Transform bindTransform = mat4ToTransform(bindMatrix);
			// 根据结点设置绑定变换
			cgltf_node* jointNode = skin->joints[j];
			int jointIndex = GLTFHelpers::GetNodeIndex(jointNode, data->nodes, numBones);
			worldBindPose[jointIndex] = bindTransform;
		}
	}

	// 将世界变换转为相对于父节点的局部变换
	Pose bindPose = restPose;
	for (unsigned int i = 0; i < numBones; i++) {
		Transform current = worldBindPose[i];
		int p = bindPose.GetParent(i);
		if (p >= 0) {
			Transform parent = worldBindPose[p];
			current = combine(inverse(parent), current); // 父节点的逆矩阵乘该结点的变换就可以取消父节点的变换
		}
		bindPose.SetLocalTransform(i, current);
	}

	return bindPose;
}

Skeleton LoadSkeleton(cgltf_data* data) {
	return Skeleton(LoadRestPose(data), LoadBindPose(data), LoadJointNames(data));
}

// 该函数仅支持只有一个模型的情况
std::vector<Mesh> LoadMeshes(cgltf_data* data) {
	std::vector<Mesh> result;
	cgltf_node* nodes = data->nodes;
	unsigned int nodeCount = (unsigned int)data->nodes_count;

	for (unsigned int i = 0; i < nodeCount; i++) {
		cgltf_node* node = &node[i];
		// 如果该节点没有mesh或skin就先跳过
		if (node->mesh == 0 || node->skin == 0) {
			continue;
		}

		// gltf中一个mesh对应一个primitive。
		unsigned int numPrims = (unsigned int)node->mesh->primitives_count;
		for (unsigned int j = 0; j < numPrims; j++) {
			result.push_back(Mesh());
			Mesh& mesh = result[result.size() - 1];

			cgltf_primitive* primitive = &node->mesh->primitives[j];

			unsigned int numAttributes = (unsigned int)primitive->attributes_count;
			for (unsigned int k = 0; k < numAttributes; k++) {
				cgltf_attribute* attribute = &primitive->attributes[k];
				GLTFHelpers::MeshFromAttribute(mesh, *attribute, node->skin, nodes, nodeCount);
			}

			if (primitive->indices != 0) {
				// 如果有索引下标，还要填充IndexBuffer
				unsigned int indexCount = primitive->indices->count;
				std::vector<unsigned int>& indices = mesh.GetIndices();
				indices.resize(indexCount);

				for (unsigned int k = 0; k < indexCount; k++) {
					indices[k] = (unsigned int)cgltf_accessor_read_index(primitive->indices, k);
				}
			}
			mesh.UpdateOpenGLBuffers();
		}
	}
}