/*Copyright reserved by KenLee@2020 ken4000kl@gmail.com*/
#include "NeneEngine/Debug.h"
#include "LappedTexturePatch.h"

#define PRECISION 1000000

using namespace std;

/** >>> Util Functions >>> */

NNVec3 Barycentric2Cartesian(const NNVec3& a, const NNVec3& b, const NNVec3& c, const NNVec3& barycentric)
{
	return NNVec3(a * barycentric.x + b * barycentric.y + c * barycentric.z);
}

void CalcTangentAndBitangent(const NNVec3& a, const NNVec3& b, const NNVec3& c, const NNVec3& normal, NNVec3& tangent, NNVec3& bitangent)
{
	static const NNVec3 up(0.0f, 1.0f, 0.0f);
	tangent = NNNormalize(up - (NNDot(up, normal) * normal));
	bitangent = NNNormalize(NNCross(tangent, normal));
}

NNUInt FindAdjacentFace(const std::tuple<NNUInt, NNUInt>& edge, const std::set<NNUInt> candidates, const std::vector<NNUInt>& indices, const std::vector<Vertex>& vertices)
{
	return 0;
}

/** <<< Util Functions <<< */

LappedTexturePatch::LappedTexturePatch()
{

}

LappedTexturePatch::~LappedTexturePatch()
{

}

void LappedTexturePatch::Grow(std::set<NNUInt>& candidate_faces)
{
	//
	NNUInt a = m_outer_edges.front();
	m_outer_edges.pop_front();
	NNUInt b = m_outer_edges.front();
	m_outer_edges.pop_front();
	//
	std::vector<NNUInt>& indices = m_source_mesh->GetIndexData();
	std::vector<Vertex>& vertices = m_source_mesh->GetVertexData();
	//
	NNUInt face = FindAdjacentFace(std::make_tuple(a, b), candidate_faces, m_indices, vertices);
	candidate_faces.erase(face);
	//
	AddFace(face);
}

void LappedTexturePatch::DrawMesh(const NNDrawMode mode)
{
	m_patch_mesh->SetDrawMode(mode);
	m_patch_mesh->Draw();
}

void LappedTexturePatch::DrawEdge(const NNDrawMode mode)
{
	m_outer_edge_mesh->SetDrawMode(mode);
	m_outer_edge_mesh->Draw();
}

void LappedTexturePatch::AddFace(NNUInt face)
{
	//
	std::vector<NNUInt>& indices = m_source_mesh->GetIndexData();
	std::vector<Vertex>& vertices = m_source_mesh->GetVertexData();
	//
	NNUInt ia = indices[face * 3 + 0];
	NNUInt ib = indices[face * 3 + 1];
	NNUInt ic = indices[face * 3 + 2];
	//
	m_indices.push_back(ia);
	m_indices.push_back(ib);
	m_indices.push_back(ic);
	//
	m_outer_edges.push_back(ia);
	m_outer_edges.push_back(ib);

	m_outer_edges.push_back(ib);
	m_outer_edges.push_back(ic);

	m_outer_edges.push_back(ic);
	m_outer_edges.push_back(ia);
	// Randomly choose a point in the face (!TODO: This is actually not uniform)
	// NNVec3 barycentric(float(rand() % PRECISION) / float(PRECISION), float(rand() % PRECISION) / float(PRECISION), float(rand() % PRECISION) / float(PRECISION));
	// NNVec3 center = Barycentric2Cartesian(vertices[ia].m_position, vertices[ib].m_position, vertices[ic].m_position, barycentric);
	NNVec3 center = (vertices[ia].m_position + vertices[ib].m_position + vertices[ic].m_position) / 3.0f;
	// Calculate tagent and bitangent
	NNVec3 tagent, bitangent, normal;
	normal = (vertices[ia].m_normal + vertices[ib].m_normal + vertices[ic].m_normal) / 3.0f;
	CalcTangentAndBitangent(vertices[ia].m_position, vertices[ib].m_position, vertices[ic].m_position, normal, tagent, bitangent);
	// Form the texcoord of the vertices ( w is basically zero )
	NNMat3 tbn(tagent, bitangent, normal);
	NNMat3 tbn_inversed = glm::inverse(tbn);
	NNVec3 center_in_tbn = tbn_inversed * center;
	NNVec3 vertex_a_in_tbn = tbn_inversed * vertices[ia].m_position - center_in_tbn;
	NNVec3 vertex_b_in_tbn = tbn_inversed * vertices[ib].m_position - center_in_tbn;
	NNVec3 vertex_c_in_tbn = tbn_inversed * vertices[ic].m_position - center_in_tbn;
	//
	vertices[ia].m_texcoord = (NNVec2(vertex_a_in_tbn.x, vertex_a_in_tbn.y) + NNVec2(0.5f, 0.5f));
	vertices[ib].m_texcoord = (NNVec2(vertex_b_in_tbn.x, vertex_b_in_tbn.y) + NNVec2(0.5f, 0.5f));
	vertices[ic].m_texcoord = (NNVec2(vertex_c_in_tbn.x, vertex_c_in_tbn.y) + NNVec2(0.5f, 0.5f));
	//
	UpdateForRendering();
}

void LappedTexturePatch::Initialize(std::shared_ptr<Mesh> source_mesh, std::set<NNUInt>& candidate_faces)
{
	// Reference the mesh
	m_source_mesh = source_mesh;
	// Randomly choose a face
	NNUInt face = rand() % NNUInt(candidate_faces.size());
	candidate_faces.erase(face);
	// Add face for initialization
	AddFace(face);
	//
	dLog("[Patch] Initialze choose face: %d; Remain: %d;\n", face, candidate_faces.size());
}

void LappedTexturePatch::UpdateForRendering()
{
	// Patch mesh
	std::vector<NNUInt> indices = m_indices;
	std::vector<Vertex> vertices = m_source_mesh->GetVertexData();
	std::vector<std::tuple<std::shared_ptr<Texture2D>, NNTextureType>> textures;
	m_patch_mesh = Mesh::Create(vertices, indices, textures);
	// Outer Edges
	indices = std::vector<NNUInt>(m_outer_edges.begin(), m_outer_edges.end());
	vertices = m_source_mesh->GetVertexData();
	m_outer_edge_mesh = Mesh::Create(vertices, indices, textures);
}