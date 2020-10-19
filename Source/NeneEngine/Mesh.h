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
	NNVec3 mPosition;
	NNVec3 mNormal;
	NNVec2 mTexcoord;
};

//
//    Mesh:
//
class Mesh 
{
public:
	~Mesh();
	static std::shared_ptr<Mesh> Create(std::vector<Vertex>& vertices, std::vector<NNUInt>& indices,
		std::vector<std::tuple<std::shared_ptr<Texture2D>, NNTextureType>>& textures);
	void Draw();
	void DrawInstance();

	//
	std::vector<NNUInt>& GetIndexData();
	std::vector<Vertex>& GetVertexData();

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

