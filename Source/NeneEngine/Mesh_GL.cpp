/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifdef NENE_GL

#include "Mesh.h"
#include "Debug.h"

using namespace std;

/** GL Implementation >>> */

class MeshImpl
{
public:
	//
	~MeshImpl();
	MeshImpl(GLuint vao, GLuint vbo, GLuint ebo, GLuint index_num, GLuint vertex_num);
	//
	void Draw();
public:
	//
	GLuint m_vao;
	GLuint m_vbo;
	GLuint m_ebo;
	//
	GLuint m_index_num;
	GLuint m_vertex_num;
	//
	NNDrawMode m_draw_mode;
};

MeshImpl::MeshImpl(GLuint vao, GLuint vbo, GLuint ebo, GLuint index_num, GLuint vertex_num)
	: m_vao(vao), m_vbo(vbo), m_ebo(ebo), m_index_num(index_num), m_vertex_num(vertex_num), m_draw_mode(NNDrawMode::NN_TRIANGLE)
{}

MeshImpl::~MeshImpl()
{
	if (m_ebo != 0) glDeleteBuffers(1, &m_ebo);
	if (m_vbo != 0) glDeleteBuffers(1, &m_vbo);
	if (m_vao != 0) glDeleteVertexArrays(1, &m_vao);
}

void MeshImpl::Draw()
{
	glBindVertexArray(m_vao);
	{
		if (m_ebo != 0)
		{
			glDrawElements(m_draw_mode, m_index_num, GL_UNSIGNED_INT, 0);
		}
		else
		{
			glDrawArrays(m_draw_mode, 0, m_vertex_num);
		}
	}
	glBindVertexArray(0);
}

/** GL Implementation <<< */

Mesh::~Mesh() 
{
	if (m_impl != nullptr)
	{
		delete m_impl;
		m_impl = nullptr;
	}
}

shared_ptr<Mesh> Mesh::Create(const vector<Vertex>& vertices)
{
	//
	GLuint vao, vbo, ebo;
	// 
	glGenBuffers(1, &(vbo));
	glGenVertexArrays(1, &(vao));
	// 
	glBindVertexArray(vao);
	{
		// VBO
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
		// POS
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(0 * sizeof(GLfloat)));
		glEnableVertexAttribArray(0);
		// NORMAL
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		// TEXCOORD
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
	}
	glBindVertexArray(0);
	//
	Mesh* result = new Mesh();
	//
	result->m_impl = new MeshImpl(vao, vbo, 0, 0, (NNUInt)vertices.size());
	//
	result->m_vertices = vertices;
	//
	return shared_ptr<Mesh>(result);
}

shared_ptr<Mesh> Mesh::Create(const vector<Vertex>& vertices, const vector<NNUInt>& indices,
	const vector<tuple<shared_ptr<Texture2D>, NNTextureType>>& textures)
{
	//
	GLuint vao, vbo, ebo;
	// 
	glGenBuffers(1, &(vbo));
	glGenBuffers(1, &(ebo));
	glGenVertexArrays(1, &(vao));
	// 
	glBindVertexArray(vao);
	{
		// VBO
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
		// EBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
		// POS
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(0 * sizeof(GLfloat)));
		glEnableVertexAttribArray(0);
		// NORMAL
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		// TEXCOORD
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
	}
	glBindVertexArray(0);
	//
	if (vao == 0)
	{
		dLog("[Error] Failed to create vertex buffer.");
		return nullptr;
	}
	//
	Mesh* result = new Mesh();
	result->m_impl = new MeshImpl(vao, vbo, ebo, (NNUInt)indices.size(), (NNUInt)vertices.size());
	//
	result->m_indices = indices;
	result->m_vertices = vertices;
	result->m_textures = textures;
	//
	return shared_ptr<Mesh>(result);
}

void Mesh::Draw() 
{
	// 绑定纹理贴图
	for (NNUInt i = 0; i < m_textures.size(); ++i) 
	{
		std::get<0>(m_textures[i])->Use(std::get<1>(m_textures[i]));
	}
	// 绘制网格数据
	m_impl->Draw();
}

void Mesh::DrawInstance()
{

}

void Mesh::SetDrawMode(const NNDrawMode mode)
{
	m_impl->m_draw_mode = mode;
}

#endif