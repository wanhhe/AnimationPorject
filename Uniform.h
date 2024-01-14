#ifndef _H_UNIFORM_
#define _H_UNIFORM_

#include <vector>

template<typename T>
// ������Զ����ʵ����
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
	static void Set(unsigned int slot, T* inputArray, int length);
	static void Set(unsigned int slot, std::vector<T>& inputArray);
};

#endif // !_H_UNIFORM_
