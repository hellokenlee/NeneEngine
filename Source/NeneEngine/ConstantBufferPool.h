/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef CONSTANT_BUFFER_POOL_H
#define CONSTANT_BUFFER_POOL_H

#include "Utils.h"
#include <vector>
#include <type_traits>

//
//  ConstantBufferPool:
//

class ConstantBufferPool {
public:
	// 构造
	ConstantBufferPool();
	// 析构
	~ConstantBufferPool();
	// 变量总数
	inline size_t len() { return mSizes.size(); }
	// 某变量长度
	inline size_t size(const NNUInt& idx) { return mSizes[idx]; }
	// 写入数据
	void writeRaw(const NNUInt& idx, const void* data);
	// 增加数据
	size_t appendRaw(const NNUInt& size, const void* data = nullptr);
	// 更新数据
	void Update(const NNUInt& slot);

	//
	template<
		typename T,
		typename = typename std::enable_if<
			std::is_same<T, int>::value || 
			std::is_same<T, float>::value || 
			std::is_same<T, NNVec2>::value || 
			std::is_same<T, NNVec3>::value ||
			std::is_same<T, NNVec4>::value
		>::type
	>
	void inline write(const NNUInt& idx, const T& data) { writeRaw(idx, &data); }
	//
	template<
		typename T,
		typename = typename std::enable_if<
			std::is_same<T, int>::value || 
			std::is_same<T, float>::value ||
			std::is_same<T, NNVec2>::value ||
			std::is_same<T, NNVec3>::value ||
			std::is_same<T, NNVec4>::value
		>::type
	>
	size_t inline append(const T& data) { return appendRaw(sizeof(T), &data); }

private:
	// 重新分配
	void reallocate(const size_t& newCapacity);
	// 禁止拷贝
	ConstantBufferPool(const ConstantBufferPool& rhs);
	ConstantBufferPool& operator=(const ConstantBufferPool& rhs);
private:
	// 内存数据
	NNByte* mBuffer;
	// 实际申请的容量
	size_t mCapacity;
	// 每一段数据大小
	std::vector<NNUInt> mSizes;
	// 每一段数据偏移
	std::vector<NNUInt> mOffsets;
	// 缓冲指针
#if defined NENE_GL
	GLuint mUBO;
#elif defined NENE_DX
	ID3D11Buffer* mpBuffer;
#endif
};


#endif // CONSTANT_BUFFER_POOL_H
