#ifndef _H_UNIFORM_
#define _H_UNIFORM_

#include <vector>

template<typename T>
// 该类永远不会实例化
class Uniform
{
private:
	// 由于要禁止实例化，所以禁用这些函数
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
