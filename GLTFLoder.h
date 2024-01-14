#ifndef _H_GLTFLOADER_H
#define _H_GLTFLOADER_H

#include "cgltf.h"

cgltf_data* LoadGLTFFile(const char* path);
void FreeGLTFFile(cgltf_data* handle);

#endif // !_H_GLTFLOADER_H
