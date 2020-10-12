/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#if defined NENE_GL
#ifndef CONSTANT_BUFFER_GL_INL
#define CONSTANT_BUFFER_GL_INL

#include <cstring>

template<typename T, std::size_t N>
ConstantBuffer<T, N>::ConstantBuffer() {
	m_UBO = 0;
	// 申请UBO显存
	glGenBuffers(1, &m_UBO);
	// 写入数据
	glBindBuffer(GL_UNIFORM_BUFFER, m_UBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(T) * N, &m_datas, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

template<typename T, std::size_t N>
ConstantBuffer<T, N>::~ConstantBuffer() {
	if (m_UBO != 0) {
		glDeleteBuffers(1, &m_UBO);
	}
}

template<typename T, std::size_t N>
void ConstantBuffer<T, N>::Update(const NNUInt& slot) {
	// 更新数据
	glBindBuffer(GL_UNIFORM_BUFFER, m_UBO);
		GLvoid *p_data = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
		memcpy(p_data, &m_datas, sizeof(m_datas) * N);
		glUnmapBuffer(GL_UNIFORM_BUFFER);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	// 绑定到某个 Slot 中
	glBindBufferBase(GL_UNIFORM_BUFFER, slot, m_UBO);
}

#endif // CONSTANT_BUFFER_GL_INL
#endif // NENE_GL