/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifdef NENE_GL

#include "Debug.h"
#include "Utils.h"
#include "ConstantBufferPool.h"

using namespace std;

ConstantBufferPool::ConstantBufferPool() 
{
	//
	mUBO = 0;
	mCapacity = 0;
	mBuffer = nullptr;
	//
	reallocate(16);
}

ConstantBufferPool::~ConstantBufferPool() 
{
	if (mUBO != 0)
	{
		glDeleteBuffers(1, &mUBO);
		mUBO = 0;
	}
}

size_t ConstantBufferPool::appendRaw(const NNUInt& size, const void* data)
{
	// 计算已经使用的大小
	NNUInt used = 0;
	if (!mOffsets.empty())
	{
		used = mOffsets.back() + mSizes.back();
	}
	// 如果不够用 重新申请内存
	if (used + size > mCapacity)
	{
		reallocate(mCapacity * 2);
	}
	// 存储新的数据的大小和偏移
	size_t idx = mSizes.size();
	mSizes.push_back(size);
	mOffsets.push_back(used);
	// 写入新的数据
	memcpy(mBuffer + used, data, size);
	//
	return idx;
}

void ConstantBufferPool::writeRaw(const NNUInt& idx, const void* data)
{
	if (idx >= mOffsets.size())
	{
		dLog("Invalid offset.");
	}
	NNUInt size = mSizes[idx];
	NNUInt offset = mOffsets[idx];
	memcpy(mBuffer + offset, data, size);
}

void ConstantBufferPool::Update(const NNUInt& slot)
{
	//
	static size_t total = 0;
	if (mOffsets.empty())
	{
		total = 0;
	}
	else
	{
		total = mOffsets.back() + mSizes.back();
	}
	// 更新数据
	glBindBuffer(GL_UNIFORM_BUFFER, mUBO);
		GLvoid *pData = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
		memcpy(pData, mBuffer, total);
		glUnmapBuffer(GL_UNIFORM_BUFFER);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	// 绑定到某个 Slot 中
	glBindBufferBase(GL_UNIFORM_BUFFER, slot, mUBO);
}

void ConstantBufferPool::reallocate(const size_t& newCapacity)
{
	//
	NNByte* newBuffer = new NNByte[newCapacity];
	memset(newBuffer, 0xffff, newCapacity);
	if (mBuffer != nullptr)
	{
		memcpy(newBuffer, mBuffer, mCapacity);
		delete mBuffer;
	}
	mBuffer = newBuffer;
	mCapacity = newCapacity;
	//
	if (mUBO != 0)
	{
		glDeleteBuffers(1, &mUBO);
	}
	//
	glGenBuffers(1, &mUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, mUBO);
		glBufferData(GL_UNIFORM_BUFFER, mCapacity, mBuffer, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

#endif // NENE_GL