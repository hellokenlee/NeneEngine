/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#if defined NENE_GL
#ifndef CONSTANT_BUFFER_GL_INL
#define CONSTANT_BUFFER_GL_INL

#include <cstring>

template<typename T>
ConstantBuffer<T>::ConstantBuffer() {
	mUBO = 0;
	// 申请UBO显存
	glGenBuffers(1, &mUBO);
	// 写入数据
	glBindBuffer(GL_UNIFORM_BUFFER, mUBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(T), &data, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

template<typename T>
ConstantBuffer<T>::~ConstantBuffer() {
	if (mUBO != 0) {
		glDeleteBuffers(1, &mUBO);
	}
}

template<typename T>
void ConstantBuffer<T>::Update(const NNUInt& slot) {
	// 更新数据
	glBindBuffer(GL_UNIFORM_BUFFER, mUBO);
		GLvoid *pData = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
		memcpy(pData, &data, sizeof(data));
		glUnmapBuffer(GL_UNIFORM_BUFFER);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	// 绑定到某个 Slot 中
	glBindBufferBase(GL_UNIFORM_BUFFER, slot, mUBO);
}

#endif // CONSTANT_BUFFER_GL_INL
#endif // NENE_GL