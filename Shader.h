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

	void Bind(); // ʹ��Shaderǰ��Ҫ��
	void UnBind(); // ������Ҫʹ�ú���
	unsigned int GetAttributes(const std::string name);
	unsigned int GetUniforms(const std::string name);
	unsigned int GetHandle(); // ����OpenGL���

private:
	unsigned int mHandle; // OpenGL�ľ��
	std::map<std::string, unsigned int> mAttributes;
	std::map<std::string, unsigned int> mUniforms;

	// ��������и���
	Shader(const Shader&);
	Shader& operator=(const Shader&);

	std::string ReadFile(const std::string& path);
	unsigned int CompileVertexShader(const std::string& vertex); // ����Shader�����ؾ��
	unsigned int CompileFragmentShader(const std::string& fragment); // ����Shader�����ؾ��
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

