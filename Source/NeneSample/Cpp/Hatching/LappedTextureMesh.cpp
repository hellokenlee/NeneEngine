/*Copyright reserved by KenLee@2020 ken4000kl@gmail.com*/

#include "LappedTextureMesh.h"

LappedTextureMesh::~LappedTextureMesh() 
{}

LappedTextureMesh::LappedTextureMesh(std::shared_ptr<StaticMesh> static_mesh):
	m_source(*static_mesh)
{

}

const LappedTexturePatch& LappedTextureMesh::AddPatch()
{
	//
	const std::shared_ptr<Mesh> mesh = m_source.GetMeshes()[0];
	LappedTexturePatch patch(mesh->GetIndexData(), mesh->GetVertexData(), m_candidate_faces);
	//
	m_patches.emplace_back(patch);
}
