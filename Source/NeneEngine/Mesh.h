/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef MESH_H
#define MESH_H

#include "Shader.h"
#include "Texture2D.h"
#include <vector>

class MeshImpl;

//
//    Vertex:
//
struct Vertex 
{
	NNVec3 m_position;
	NNVec3 m_normal;
	NNVec2 m_texcoord;
};

//
//    Mesh:
//
class Mesh 
{
public:
	//
	~Mesh();
	//
	static std::shared_ptr<Mesh> Create(const std::vector<Vertex>& vertices);
	static std::shared_ptr<Mesh> Create(const std::vector<Vertex>& vertices, const std::vector<NNUInt>& indices,
		const std::vector<std::tuple<std::shared_ptr<Texture2D>, NNTextureType>>& textures);
	//
	void Draw();
	void DrawInstance();
	//
	void SetDrawMode(const NNDrawMode mode);
	//
	std::vector<NNUInt>& GetIndexData() { return m_indices; }
	std::vector<Vertex>& GetVertexData() { return m_vertices; };
	//
	const std::vector<NNUInt>& GetIndexData() const { return m_indices; }
	const std::vector<Vertex>& GetVertexData() const { return m_vertices; }

protected:
	//
	MeshImpl* m_impl;
	//
	NNVertexFormat m_vertex_format;
	//
	std::vector<NNUInt> m_indices;
	std::vector<Vertex> m_vertices;
	//
	std::vector<std::tuple<std::shared_ptr<Texture2D>, NNTextureType>> m_textures;

protected:
	//
	Mesh() = default;
	Mesh(const Mesh& rhs) = delete;
	Mesh& operator=(const Mesh& rhs) = delete;

protected:
	friend class Model;
};

#endif // !MESH_H

