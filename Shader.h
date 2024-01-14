#ifndef _H_SHADER_
#define _H_SHADER_

#include <map>
#include <string>

class Shader
{
public:
	Shader();
	Shader(const std::string vertex, const std::string fragment);
	~Shader();
	void Load(const std::string vertex, const std::string fragment);

	void Bind(); // 使用Shader前需要绑定
	void UnBind(); // 不再需要使用后解绑
	unsigned int GetAttributes(const std::string name);
	unsigned int GetUniforms(const std::string name);
	unsigned int GetHandle(); // 返回OpenGL句柄

private:
	unsigned int mHandle; // OpenGL的句柄
	std::map<std::string, unsigned int> mAttributes;
	std::map<std::string, unsigned int> mUniforms;

	// 不允许进行复制
	Shader(const Shader&);
	Shader& operator=(const Shader&);

	std::string ReadFile(const std::string& path);
	unsigned int CompileVertexShader(const std::string& vertex); // 编译Shader并返回句柄
	unsigned int CompileFragmentShader(const std::string& fragment); // 编译Shader并返回句柄
	bool LinkShaders(unsigned int vertex, unsigned int fragment);
	void PopulateAttributes();
	void PopulateUniforms();
};

Shader::Shader()
{
}

Shader::~Shader()
{
}

#endif // !_H_SHADER_

