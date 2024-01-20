#ifndef _H_UNIFORM_
#define _H_UNIFORM_

#include <vector>

 // ������Զ����ʵ����
template<typename T>
class Uniform
{
private:
	// ����Ҫ��ֹʵ���������Խ�����Щ����
	Uniform();
	Uniform(const Uniform&);
	Uniform& operator=(const Uniform&);
	~Uniform();

public:
	static void Set(unsigned int slot, const T& value);
	static void Set(unsigned int slot, T* inputArray, unsigned int length);
	static void Set(unsigned int slot, std::vector<T>& inputArray);
};

#endif // !_H_UNIFORM_