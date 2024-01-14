#include "Texture.h"
#include "glad.h"
#include "stb_image.h"

Texture::Texture() {
	unsigned int mWidth = 0;
	unsigned int mHeight = 0;
	unsigned int mChannels = 0;
	glGenTextures(1, &mHandle);
}

Texture::Texture(const char* path) {
	glGenTextures(1, &mHandle);
	Load(path);
}

Texture::~Texture() {
	glDeleteTextures(1, &mHandle);
}

void Texture::Load(const char* path) {
	glBindTexture(GL_TEXTURE_2D, mHandle); // 绑定

	int width, height, channels;
	unsigned char* data = stbi_load(path, &width, &height, &channels, 4);
	// 使用载入的图片生成纹理
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);

	// 对S轴和T轴设置超出纹理坐标的效果
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// 纹理被放大或缩小时所使用的过滤选项
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// 恢复初态
	glBindTexture(GL_TEXTURE_2D, 0);

	mWidth = width;
	mHeight = height;
	mChannels = channels;
}

void Texture::Set(unsigned int uniformIndex, unsigned int textureIndex) {
	// 激活纹理单元
	glActiveTexture(GL_TEXTURE0 + textureIndex);
	// 将纹理绑定到该单元上
	glBindTexture(GL_TEXTURE_2D, mHandle);

	// 给sampler2D传值
	glUniform1i(uniformIndex, textureIndex);
}

void Texture::UnSet(unsigned int textureIndex) {
	glActiveTexture(GL_TEXTURE0 + textureIndex);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
}

unsigned int Texture::GetHandle() {
	return mHandle;
}