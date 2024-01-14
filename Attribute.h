#ifndef _H_ATTRIBUTE_
#define _H_ATTRIBUTE_

#include <vector>

template<typename T>
class Attribute {
protected:
	unsigned int mHandle; // VBO的句柄
	unsigned int mCount;

private:
	// 不需要多个句柄，禁用复制和赋值
	Attribute(const Attribute& other);
	Attribute& operator=(const Attribute& other);
	void SetAttribPointer(unsigned int slot); // 根据顶点的在第几个location配置VAO

public:
	Attribute();
	~Attribute();

	void Set(T* inputArray, unsigned int arrayLength);
	void Set(std::vector<T>& input);

	void BindTo(unsigned int slot); // 将VAO绑定到指定的位置上
	void UnBindFrom(unsigned int slot); // 解绑

	unsigned int Count();
	unsigned int GetHandle();
};

#endif // !_H_ATTRIBUTE_

