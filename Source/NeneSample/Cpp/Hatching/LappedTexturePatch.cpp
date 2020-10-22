/*Copyright reserved by KenLee@2020 ken4000kl@gmail.com*/
#include <optional>

#include "NeneEngine/Debug.h"
#include "LappedTexturePatch.h"

#define PRECISION 1000000
#define TEXTURE_PASTING_SCALE 1.0f

using namespace std;

/** >>> Util Functions >>> */

struct Adjacency
{
	NNUInt face;
	NNUInt shared_v0, shared_v1;
};

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

bool IsNearlySame(const NNVec2& pos0, const NNVec2& pos1)
{
	auto result = glm::epsilonEqual(pos0, pos1, 0.00001f);
	return result.x && result.y;
}

bool IsNearlySame(const NNVec3& pos0, const NNVec3& pos1)
{
	auto result = glm::epsilonEqual(pos0, pos1, 0.0000001f);
	return result.x && result.y && result.z;
}



NNVec2 SimilarTriangle3DTo2D(NNVec3 a_3d, NNVec3 b_3d, NNVec3 c_3d, NNVec3 n_3d, NNVec2 a_2d, NNVec2 b_2d)
{
	//
	a_3d *= 1000.0f;
	b_3d *= 1000.0f;
	c_3d *= 1000.0f;
	//
	a_2d *= 1000.0f;
	b_2d *= 1000.0f;
	//
	NNFloat len_ab_3d = glm::distance(a_3d, b_3d);
	NNFloat len_ac_3d = glm::distance(a_3d, c_3d);
	NNFloat len_ab_2d = glm::distance(a_2d, b_2d);
	//
	NNVec3 dir_ab_3d = glm::normalize(b_3d - a_3d);
	NNVec3 dir_ac_3d = glm::normalize(c_3d - a_3d);
	//
	NNFloat angle_bac = glm::orientedAngle(dir_ab_3d, dir_ac_3d, -n_3d);
	//
	NNVec2 dir_ab_2d = glm::normalize(b_2d - a_2d);
	NNVec2 dir_ac_2d = glm::normalize(glm::rotate(dir_ab_2d, angle_bac));
	NNVec2 c_2d = a_2d + (dir_ac_2d * len_ac_3d * len_ab_2d / len_ab_3d);
	c_2d *= 0.001f;
	return c_2d;
}

optional<Adjacency> FindAdjacentFace(const std::tuple<NNUInt, NNUInt>& edge, const std::set<NNUInt> candidates, const std::vector<NNUInt>& indices, const std::vector<Vertex>& vertices)
{
	for (const NNUInt& face : candidates)
	{
		//
		const NNVec3& e0 = vertices[get<0>(edge)].m_position;
		const NNVec3& e1 = vertices[get<1>(edge)].m_position;
		//
		NNUInt ia = indices[face * 3 + 0];
		NNUInt ib = indices[face * 3 + 1];
		NNUInt ic = indices[face * 3 + 2];
		//
		const NNVec3& av = vertices[ia].m_position;
		const NNVec3& bv = vertices[ib].m_position;
		const NNVec3& cv = vertices[ic].m_position;
		// 相邻的三角形共享边的顶点顺序是相反的
		if (IsNearlySame(e1, av) && IsNearlySame(e0, bv))
		{
			return Adjacency{ face, ia, ib };
		}
		if (IsNearlySame(e1, bv) && IsNearlySame(e0, cv))
		{
			return Adjacency{ face, ib, ic };
		}
		if (IsNearlySame(e1, cv) && IsNearlySame(e0, av))
		{
			return Adjacency{ face, ic, ia };
		}
	}
	return nullopt;
}

NNVec2 CoordTagentToTexture(const NNVec2& tagnet_coord)
{
	return (tagnet_coord * TEXTURE_PASTING_SCALE) + NNVec2(0.5f, 0.5f);
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
	NNUInt ia_origin = m_outer_edges.front();
	m_outer_edges.pop_front();
	NNUInt ib_origin = m_outer_edges.front();
	m_outer_edges.pop_front();
	//
	std::vector<NNUInt>& indices = m_source_mesh->GetIndexData();
	std::vector<Vertex>& vertices = m_source_mesh->GetVertexData();
	//
	optional<Adjacency> adj = FindAdjacentFace(std::make_tuple(ia_origin, ib_origin), candidate_faces, indices, vertices);
	if (adj.has_value())
	{
		//
		NNUInt face = adj->face;
		candidate_faces.erase(face);
		//
		NNUInt ia_adjacent = indices[face * 3 + 0];
		NNUInt ib_adjacent = indices[face * 3 + 1];
		NNUInt ic_adjacent = indices[face * 3 + 2];
		m_indices.push_back(ia_adjacent);
		m_indices.push_back(ib_adjacent);
		m_indices.push_back(ic_adjacent);
		//
		NNUInt shared_ia = adj->shared_v0;
		NNUInt shared_ib = adj->shared_v1;
		assert(IsNearlySame(vertices[shared_ia].m_position, vertices[ib_origin].m_position));
		assert(IsNearlySame(vertices[shared_ib].m_position, vertices[ia_origin].m_position));
		vertices[shared_ia].m_texcoord = vertices[ib_origin].m_texcoord;
		vertices[shared_ib].m_texcoord = vertices[ia_origin].m_texcoord;
		//
		NNUInt remaining_ic;
		if (shared_ia == ia_adjacent && shared_ib == ib_adjacent)
		{
			remaining_ic = ic_adjacent;
		}
		else if (shared_ia == ib_adjacent && shared_ib == ic_adjacent)
		{
			remaining_ic = ia_adjacent;
		}
		else if (shared_ia == ic_adjacent && shared_ib == ia_adjacent)
		{
			remaining_ic = ib_adjacent;
		}
		else
		{
			throw std::invalid_argument("Unshared edge!");
		}
		//
		m_outer_edges.push_back(shared_ib);
		m_outer_edges.push_back(remaining_ic);
		m_outer_edges.push_back(remaining_ic);
		m_outer_edges.push_back(shared_ia);
		// Grow the triangle over the shared edge
		NNVec3 pa = vertices[shared_ia].m_position;
		NNVec3 pb = vertices[shared_ib].m_position;
		NNVec3 pc = vertices[remaining_ic].m_position;
		NNVec3 na = vertices[shared_ia].m_normal;
		NNVec3 nb = vertices[shared_ib].m_normal;
		NNVec3 nc = vertices[remaining_ic].m_normal;
		assert(IsNearlySame(na, nb) && IsNearlySame(nb, nc));
		NNVec2 ta = vertices[shared_ia].m_texcoord;
		NNVec2 tb = vertices[shared_ib].m_texcoord;
		NNVec2 tc = SimilarTriangle3DTo2D(pa, pb, pc, na,ta, tb);
		//
		vertices[remaining_ic].m_texcoord = tc;
		//
		UpdateForRendering();
		// 
		dLog("[Patch] Grow add face: %d; Remain: %zd faces;\n", face, candidate_faces.size());
	}
	else
	{
		dLog("[Error] Failed to grow patch! Unable to find adjacent face!\n");
	}
}

void LappedTexturePatch::DrawMesh()
{
	m_patch_mesh->Draw();
}

void LappedTexturePatch::DrawEdge()
{
	m_outer_edge_mesh->Draw();
}

void LappedTexturePatch::Initialize(std::shared_ptr<Mesh> source_mesh, std::set<NNUInt>& candidate_faces)
{
	// Reference the mesh
	m_source_mesh = source_mesh;
	// Randomly choose a face
	/* 
	NNUInt face = rand() % NNUInt(candidate_faces.size());
	*/
	NNUInt face = *(candidate_faces.begin());
	candidate_faces.erase(face);
	// Add face for initialization
	std::vector<NNUInt>& indices = m_source_mesh->GetIndexData();
	std::vector<Vertex>& vertices = m_source_mesh->GetVertexData();
	//
	NNUInt ia = indices[face * 3 + 0];
	NNUInt ib = indices[face * 3 + 1];
	NNUInt ic = indices[face * 3 + 2];
	m_indices.push_back(ia);
	m_indices.push_back(ib);
	m_indices.push_back(ic);
	m_outer_edges.push_back(ia);
	m_outer_edges.push_back(ib);
	m_outer_edges.push_back(ib);
	m_outer_edges.push_back(ic);
	m_outer_edges.push_back(ic);
	m_outer_edges.push_back(ia);
	//
	// Randomly choose a point in the face (!TODO: This is actually not uniform)
	/* 
	NNVec3 barycentric(float(rand() % PRECISION) / float(PRECISION), float(rand() % PRECISION) / float(PRECISION), float(rand() % PRECISION) / float(PRECISION));
	NNVec3 center = Barycentric2Cartesian(vertices[ia].m_position, vertices[ib].m_position, vertices[ic].m_position, barycentric);
	*/
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
	vertices[ia].m_texcoord = CoordTagentToTexture(NNVec2(vertex_a_in_tbn.x, vertex_a_in_tbn.y));
	vertices[ib].m_texcoord = CoordTagentToTexture(NNVec2(vertex_b_in_tbn.x, vertex_b_in_tbn.y));
	vertices[ic].m_texcoord = CoordTagentToTexture(NNVec2(vertex_c_in_tbn.x, vertex_c_in_tbn.y));
	//
	UpdateForRendering();
	//
	dLog("[Patch] Initialze choose face: %d; Remain: %zd faces;\n", face, candidate_faces.size());
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
	m_outer_edge_mesh->SetDrawMode(NNDrawMode::NN_LINE);
}