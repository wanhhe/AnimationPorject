#include "GLTFLoder.h"
#include <iostream>

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