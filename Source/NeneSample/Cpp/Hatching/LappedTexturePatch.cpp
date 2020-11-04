/*Copyright reserved by KenLee@2020 ken4000kl@gmail.com*/
#include <optional>
#include <iostream>
#include "NeneEngine/Debug.h"
#include "LappedTexturePatch.h"

#define PRECISION 1000000
#define TEXTURE_PASTING_SCALE 1.0f

using namespace std;
using namespace glm;
using namespace Eigen;


/** >>> Util Functions >>> */

Matrix2x3f CalcLinearTransformWithEigen(const Vector3f& a, const Vector3f& b, const Vector3f& c, const Vector2f& ta, const Vector2f& tb, const Vector2f& tc)
{
	//
	static const Vector3f e0(1.f, 0.f, 0.0f);
	static const Vector3f e1(0.f, 1.f, 0.0f);
	static const Vector3f e2(0.f, 0.f, 1.0f);
	//
	Matrix3f lsm;
	lsm << a, b, c;
	//
	Vector3f xa = lsm.colPivHouseholderQr().solve(e0);
	Vector2f col0 = xa[0] * ta + xa[1] * tb + xa[2] * tc;

	Vector3f xb = lsm.colPivHouseholderQr().solve(e1);
	Vector2f col1 = xb[0] * ta + xb[1] * tb + xb[2] * tc;

	Vector3f xc = lsm.colPivHouseholderQr().solve(e2);
	Vector2f col2 = xc[0] * ta + xc[1] * tb + xc[2] * tc;
	//
	Matrix2x3f phi;
	phi << col0, col1, col2;
	//
	return phi;
}

Vector3f ToVector3f(const vec3& a)
{
	return Vector3f(a.x, a.y, a.z);
}

Vector2f ToVector2f(const vec2& a)
{
	return Vector2f(a.x, a.y);
}

enum IntersectStatus
{
	INSIDE = 0,
	INTERSECTING,
	OVERLAPPING,
	TOUCHING,
	OUTSIDE,
};

/* Check whether P and Q lie on the same side of line AB */
float Side(NNVec2 p, NNVec2 q, NNVec2 a, NNVec2 b)
{
	float z1 = (b.x - a.x) * (p.y - a.y) - (p.x - a.x) * (b.y - a.y);
	float z2 = (b.x - a.x) * (q.y - a.y) - (q.x - a.x) * (b.y - a.y);
	return z1 * z2;
}

/* Check whether segment P0P1 intersects with triangle t0t1t2 */
IntersectStatus Intersect(NNVec2 p0, NNVec2 p1, NNVec2 t0, NNVec2 t1, NNVec2 t2)
{
	/* Check whether segment is outside one of the three half-planes
	 * delimited by the triangle. */
	float f1 = Side(p0, t2, t0, t1), f2 = Side(p1, t2, t0, t1);
	float f3 = Side(p0, t0, t1, t2), f4 = Side(p1, t0, t1, t2);
	float f5 = Side(p0, t1, t2, t0), f6 = Side(p1, t1, t2, t0);
	/* Check whether triangle is totally inside one of the two half-planes
	 * delimited by the segment. */
	float f7 = Side(t0, t1, p0, p1);
	float f8 = Side(t1, t2, p0, p1);

	/* If segment is strictly outside triangle, or triangle is strictly
	 * apart from the line, we're not intersecting */
	if ((f1 < 0 && f2 < 0) || (f3 < 0 && f4 < 0) || (f5 < 0 && f6 < 0)
		|| (f7 > 0 && f8 > 0))
		return IntersectStatus::OUTSIDE;

	/* If segment is aligned with one of the edges, we're overlapping */
	if ((f1 == 0 && f2 == 0) || (f3 == 0 && f4 == 0) || (f5 == 0 && f6 == 0))
		return IntersectStatus::OVERLAPPING;

	/* If segment is outside but not strictly, or triangle is apart but
	 * not strictly, we're touching */
	if ((f1 <= 0 && f2 <= 0) || (f3 <= 0 && f4 <= 0) || (f5 <= 0 && f6 <= 0)
		|| (f7 >= 0 && f8 >= 0))
		return IntersectStatus::TOUCHING;

	/* If both segment points are strictly inside the triangle, we
	 * are not intersecting either */
	if (f1 > 0 && f2 > 0 && f3 > 0 && f4 > 0 && f5 > 0 && f6 > 0)
		return IntersectStatus::INSIDE;

	/* Otherwise we're intersecting with at least one edge */
	return IntersectStatus::INTERSECTING;
}

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
	//
	c_2d = clamp(c_2d, 0.0f, 1.0f);
	//
	// assert(c_2d.x <= 1.0f && c_2d.x >= 0.0f && c_2d.y <= 1.0f && c_2d.y >= 0.0f);
	return c_2d;
}

optional<Adjacency> FindAdjacentFace_SharedVertex(const std::tuple<NNUInt, NNUInt>& edge, const std::set<NNUInt> candidates, const std::vector<NNUInt>& indices, const std::vector<Vertex>& vertices)
{
	for (const NNUInt& face : candidates)
	{
		//
		NNUInt ia = indices[face * 3 + 0];
		NNUInt ib = indices[face * 3 + 1];
		NNUInt ic = indices[face * 3 + 2];
		//
		const NNUInt& i0 = get<0>(edge);
		const NNUInt& i1 = get<1>(edge);
		// 相邻的三角形共享边的顶点顺序是相反的
		if (i1 == ia && i0 == ib)
		{
			return Adjacency{ face, ia, ib, ic};
		}
		if (i1 == ib && i0 == ic)
		{
			return Adjacency{ face, ib, ic, ia };
		}
		if (i1 == ic && i0 == ia)
		{
			return Adjacency{ face, ic, ia, ib };
		}
	}
	return nullopt;
}

optional<Adjacency> FindAdjacentFace_SeparateVertex(const std::tuple<NNUInt, NNUInt>& edge, const std::set<NNUInt> candidates, const std::vector<NNUInt>& indices, const std::vector<Vertex>& vertices)
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
			return Adjacency{ face, ia, ib, ic };
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
	if (m_is_grown)
	{
		dLog("[Patch] The patch is grown.\n");
		return;
	}
	//
	vector<NNUInt>& indices = m_source_mesh->GetIndexData();
	vector<Vertex>& vertices = m_source_mesh->GetVertexData();
	//
	optional<Adjacency> adj = FindNearestAdjacentFace(candidate_faces);
	if (adj.has_value())
	{
		// 更新Patch的顶点
		NNUInt face = adj->face;
		candidate_faces.erase(face);
		m_indices.push_back(indices[face * 3 + 0]);
		m_indices.push_back(indices[face * 3 + 1]);
		m_indices.push_back(indices[face * 3 + 2]);
		m_indices_set.insert(indices[face * 3 + 0]);
		m_indices_set.insert(indices[face * 3 + 1]);
		m_indices_set.insert(indices[face * 3 + 2]);
		
		// 计算新增加的顶点的纹理坐标
		NNUInt shared_ia = adj->shared_ia;
		NNUInt shared_ib = adj->shared_ib;
		NNUInt diagonal_ic = adj->diagonal_ic;
		//
		NNVec3 pa = vertices[shared_ia].m_position;
		NNVec3 pb = vertices[shared_ib].m_position;
		NNVec3 pc = vertices[diagonal_ic].m_position;
		NNVec2 ta = vertices[shared_ia].m_texcoord;
		NNVec2 tb = vertices[shared_ib].m_texcoord;
		//
		NNVec3 na = vertices[shared_ia].m_normal;
		NNVec3 nb = vertices[shared_ib].m_normal;
		NNVec3 nc = vertices[diagonal_ic].m_normal;
		NNVec3 nf = NNNormalize((na + nb + nc) / 3.0f);
		NNVec2 tc = SimilarTriangle3DTo2D(pa, pb, pc, nf, ta, tb);
		//
		if (!IsNearlySame(vertices[diagonal_ic].m_texcoord, NNVec2(0.0f, 0.0f)))
		{
			dLog("[Patch] Recover an vertex: %d in face: %d\n", diagonal_ic, face);
		}
		//
		vertices[diagonal_ic].m_texcoord = tc;
		//
		dLog("[Patch] Grow %dth face of patch.\n", int(m_indices.size() / 3));
		dLog("[Patch] Grow add face: %d; Remain: %zd faces; Segment((%f, %f), (%f, %f))\n", face, candidate_faces.size(), ta.x, -ta.y, tb.x, -tb.y);

		// 计算线性变换矩阵
		// Matrix2x3f phi = CalcLinearTransformWithEigen(ToVector3f(pa), ToVector3f(pb), ToVector3f(pc), ToVector2f(ta), ToVector2f(tb), ToVector2f(tc));
		// m_face_phi.insert(make_pair(face, phi));

		// 合并已经覆盖的面
		std::vector<NNUInt> covered_faces;
		for (const NNUInt& c : candidate_faces)
		{
			const NNUInt ia = indices[c * 3 + 0];
			const NNUInt ib = indices[c * 3 + 1];
			const NNUInt ic = indices[c * 3 + 2];
			bool has_ia = m_indices_set.find(ia) != m_indices_set.end();
			bool has_ib = m_indices_set.find(ib) != m_indices_set.end();
			bool has_ic = m_indices_set.find(ic) != m_indices_set.end();
			// 上面新增的顶点导致这个面的三个顶点都被覆盖
			if (has_ia && has_ib && has_ic)
			{
				covered_faces.push_back(c);
				//
				m_indices.push_back(ia);
				m_indices.push_back(ib);
				m_indices.push_back(ic);
				//
				NNVec3 pa = vertices[ia].m_position;
				NNVec3 pb = vertices[ib].m_position;
				NNVec3 pc = vertices[ic].m_position;
				NNVec2 ta = vertices[ia].m_texcoord;
				NNVec2 tb = vertices[ib].m_texcoord;
				NNVec2 tc = vertices[ic].m_texcoord;
				//
				// Matrix2x3f phi = CalcLinearTransformWithEigen(ToVector3f(pa), ToVector3f(pb), ToVector3f(pc), ToVector2f(ta), ToVector2f(tb), ToVector2f(tc));
				// m_face_phi.insert(make_pair(face, phi));
				//
				NNVec3 tab(tb.x - ta.x, tb.y - ta.y, 0.0f);
				NNVec3 tbc(tc.x - tb.x, tc.y - tb.y, 0.0f);
				NNVec3 direction = cross(tab, tbc);
				// 新增的点导致了这个面和已有的面重叠
				if (direction.z < 0.0f)
				{
					//
					NNVec3 na = vertices[ia].m_normal;
					NNVec3 nb = vertices[ib].m_normal;
					NNVec3 nc = vertices[ic].m_normal;
					NNVec3 nf = NNNormalize((na + nb + nc) / 3.0f);
					//
					if (ia == diagonal_ic)
					{
						NNVec2 new_ta = SimilarTriangle3DTo2D(pb, pc, pa, nf, tb, tc);
						vertices[ia].m_texcoord = new_ta;
					}
					else if (ib == diagonal_ic)
					{
						NNVec2 new_tb = SimilarTriangle3DTo2D(pc, pa, pb, nf, tc, ta);
						vertices[ib].m_texcoord = new_tb;
					}
					else if (ic == diagonal_ic)
					{
						NNVec2 new_tc = SimilarTriangle3DTo2D(pa, pb, pc, nf, ta, tb);
						vertices[ic].m_texcoord = new_tc;
					}
					else
					{
						assert(false);
					}
					//
					dLog("!!! [Patch] Adjust face\n");
				}
				//
				dLog("[Patch] Expand covered face: %d due to grow of: %d\n", c, face);
			}
		}
		for (auto c : covered_faces)
		{
			candidate_faces.erase(c);
		}
		//
		UpdateForRendering();
	}
	else
	{
		m_is_grown = true;
		dLog("[Patch] Failed to grow patch! Unable to find adjacent face!\n");
	}
}

void LappedTexturePatch::Optimaze()
{
	for (auto&[face, phi] : m_face_phi)
	{
		cout << face <<endl << phi << endl << endl;
	}
}

void LappedTexturePatch::Initialize(std::shared_ptr<Mesh> source_mesh, std::set<NNUInt>& candidate_faces)
{
	// Reference the mesh
	m_source_mesh = source_mesh;
	//
	m_is_grown = false;
	m_indices.clear();
	m_indices_set.clear();
	// Randomly choose a face
	/* 
	NNUInt face = rand() % NNUInt(candidate_faces.size());
	*/
	NNUInt face = *(candidate_faces.begin());
	candidate_faces.erase(face);
	// Add face for initialization
	vector<NNUInt>& indices = m_source_mesh->GetIndexData();
	vector<Vertex>& vertices = m_source_mesh->GetVertexData();
	//
	const NNUInt ia = indices[face * 3 + 0];
	const NNUInt ib = indices[face * 3 + 1];
	const NNUInt ic = indices[face * 3 + 2];
	m_indices.push_back(ia);
	m_indices.push_back(ib);
	m_indices.push_back(ic);
	m_indices_set.insert(ia);
	m_indices_set.insert(ib);
	m_indices_set.insert(ic);
	//
	// Randomly choose a point in the face (!TODO: This is actually not uniform)
	/* 
	NNVec3 barycentric(float(rand() % PRECISION) / float(PRECISION), float(rand() % PRECISION) / float(PRECISION), float(rand() % PRECISION) / float(PRECISION));
	NNVec3 center = Barycentric2Cartesian(vertices[ia].m_position, vertices[ib].m_position, vertices[ic].m_position, barycentric);
	*/
	m_center = (vertices[ia].m_position + vertices[ib].m_position + vertices[ic].m_position) / 3.0f;
	// Calculate tagent and bitangent
	NNVec3 tagent, bitangent, normal;
	normal = NNNormalize((vertices[ia].m_normal + vertices[ib].m_normal + vertices[ic].m_normal) / 3.0f);
	CalcTangentAndBitangent(vertices[ia].m_position, vertices[ib].m_position, vertices[ic].m_position, normal, tagent, bitangent);
	// Form the texcoord of the vertices ( w is basically zero )
	NNMat3 tbn(tagent, bitangent, normal);
	NNMat3 tbn_inversed = glm::inverse(tbn);
	NNVec3 center_in_tbn = tbn_inversed * m_center;
	NNVec3 vertex_a_in_tbn = tbn_inversed * vertices[ia].m_position - center_in_tbn;
	NNVec3 vertex_b_in_tbn = tbn_inversed * vertices[ib].m_position - center_in_tbn;
	NNVec3 vertex_c_in_tbn = tbn_inversed * vertices[ic].m_position - center_in_tbn;
	//
	vertices[ia].m_texcoord = (NNVec2(vertex_a_in_tbn.x, vertex_a_in_tbn.y) * TEXTURE_PASTING_SCALE) + NNVec2(0.5f, 0.5f);
	vertices[ib].m_texcoord = (NNVec2(vertex_b_in_tbn.x, vertex_b_in_tbn.y) * TEXTURE_PASTING_SCALE) + NNVec2(0.5f, 0.5f);
	vertices[ic].m_texcoord = (NNVec2(vertex_c_in_tbn.x, vertex_c_in_tbn.y) * TEXTURE_PASTING_SCALE) + NNVec2(0.5f, 0.5f);
	//
	NNVec3 pa = vertices[ia].m_position;
	NNVec3 pb = vertices[ib].m_position;
	NNVec3 pc = vertices[ic].m_position;
	NNVec2 ta = vertices[ia].m_texcoord;
	NNVec2 tb = vertices[ib].m_texcoord;
	NNVec2 tc = vertices[ic].m_texcoord;
	// Matrix2x3f phi = CalcLinearTransformWithEigen(ToVector3f(pa), ToVector3f(pb), ToVector3f(pc), ToVector2f(ta), ToVector2f(tb), ToVector2f(tc));
	// m_face_phi.insert(make_pair(face, phi));
	//
	UpdateForRendering();
	//
	dLog("[Patch] Init add face: %d; Remain: %zd faces;\n", face, candidate_faces.size());
}

void LappedTexturePatch::DrawMesh()
{
	m_patch_mesh->Draw();
}

void LappedTexturePatch::UpdateForRendering()
{
	// Patch mesh
	vector<NNUInt> indices = m_indices;
	vector<Vertex> vertices = m_source_mesh->GetVertexData();
	vector<std::tuple<std::shared_ptr<Texture2D>, NNTextureType>> textures;
	m_patch_mesh = Mesh::Create(vertices, indices, textures);
}

optional<Adjacency> LappedTexturePatch::FindNearestAdjacentFace(std::set<NNUInt>& candidate_faces)
{
	//
	const vector<NNUInt>& indices = m_source_mesh->GetIndexData();
	const vector<Vertex>& vertices = m_source_mesh->GetVertexData();
	//
	NNFloat min_dis = 3.40281e+038f;
	optional<Adjacency> min_dis_adj = nullopt;
	//
	for (const auto face : candidate_faces)
	{
		const NNUInt ia = indices[face * 3 + 0];
		const NNUInt ib = indices[face * 3 + 1];
		const NNUInt ic = indices[face * 3 + 2];
		bool has_ia = m_indices_set.find(ia) != m_indices_set.end();
		bool has_ib = m_indices_set.find(ib) != m_indices_set.end();
		bool has_ic = m_indices_set.find(ic) != m_indices_set.end();
		//
		assert(int(has_ia) + int(has_ib) + int(has_ic) != 3);
		//
		optional<Adjacency> current_adj = nullopt;
		//
		if (has_ia && has_ib && !has_ic)
		{
			current_adj = Adjacency{face, ia, ib, ic};
		}
		if (has_ia && !has_ib && has_ic)
		{
			current_adj = Adjacency{ face, ic, ia, ib };
		}
		if (!has_ia && has_ib && has_ic)
		{
			current_adj = Adjacency{ face, ib, ic, ia };
		}
		//
		if (current_adj.has_value() && IsValidAdjacency(*current_adj))
		{
			const NNVec3 pa = vertices[ia].m_position;
			const NNVec3 pb = vertices[ib].m_position;
			const NNVec3 pc = vertices[ic].m_position;

			const NNVec3 center = (pa + pb + pc) / 3.0f;
			
			NNFloat dis = glm::distance(center, m_center);
			if (dis < min_dis)
			{
				min_dis = dis;
				min_dis_adj = current_adj;
			}
		}
	}
	return min_dis_adj;
}

bool LappedTexturePatch::IsValidAdjacency(const Adjacency& adjcency)
{
	//
	const NNUInt ia = adjcency.shared_ia;
	const NNUInt ib = adjcency.shared_ib;
	//
	const vector<Vertex>& vertices = m_source_mesh->GetVertexData();
	//
	const NNVec2 ta = vertices[ia].m_texcoord;
	const NNVec2 tb = vertices[ib].m_texcoord;
	//
	static const vector<tuple<NNVec2, NNVec2, NNVec2>> polygon_hull = {
		{{0.54248366 * 0.8 + 0.1, (1.0 - 0.02614379) * 0.8 + 0.1}, {0.41993464 * 0.8 + 0.1, (1.0 - 0.02941176) * 0.8 + 0.1}, {0.48202614 * 0.8 + 0.1, (1.0 - 0.37908497) * 0.8 + 0.1}},
		{{0.41993464 * 0.8 + 0.1, (1.0 - 0.02941176) * 0.8 + 0.1}, {0.35130719 * 0.8 + 0.1, (1.0 - 0.26797386) * 0.8 + 0.1}, {0.48202614 * 0.8 + 0.1, (1.0 - 0.37908497) * 0.8 + 0.1}},
		{{0.35130719 * 0.8 + 0.1, (1.0 - 0.26797386) * 0.8 + 0.1}, {0.10457516 * 0.8 + 0.1, (1.0 - 0.30228758) * 0.8 + 0.1}, {0.48202614 * 0.8 + 0.1, (1.0 - 0.37908497) * 0.8 + 0.1}},
		{{0.10457516 * 0.8 + 0.1, (1.0 - 0.30228758) * 0.8 + 0.1}, {0.05882353 * 0.8 + 0.1, (1.0 - 0.33660131) * 0.8 + 0.1}, {0.48202614 * 0.8 + 0.1, (1.0 - 0.37908497) * 0.8 + 0.1}},
		{{0.05882353 * 0.8 + 0.1, (1.0 - 0.33660131) * 0.8 + 0.1}, {0.06045752 * 0.8 + 0.1, (1.0 - 0.43464052) * 0.8 + 0.1}, {0.48202614 * 0.8 + 0.1, (1.0 - 0.37908497) * 0.8 + 0.1}},
		{{0.06045752 * 0.8 + 0.1, (1.0 - 0.43464052) * 0.8 + 0.1}, {0.17973856 * 0.8 + 0.1, (1.0 - 0.48366013) * 0.8 + 0.1}, {0.48202614 * 0.8 + 0.1, (1.0 - 0.37908497) * 0.8 + 0.1}},
		{{0.17973856 * 0.8 + 0.1, (1.0 - 0.48366013) * 0.8 + 0.1}, {0.11111111 * 0.8 + 0.1, (1.0 - 0.73529412) * 0.8 + 0.1}, {0.48202614 * 0.8 + 0.1, (1.0 - 0.37908497) * 0.8 + 0.1}},
		{{0.11111111 * 0.8 + 0.1, (1.0 - 0.73529412) * 0.8 + 0.1}, {0.14869281 * 0.8 + 0.1, (1.0 - 0.76143791) * 0.8 + 0.1}, {0.48202614 * 0.8 + 0.1, (1.0 - 0.37908497) * 0.8 + 0.1}},
		{{0.14869281 * 0.8 + 0.1, (1.0 - 0.76143791) * 0.8 + 0.1}, {0.24019608 * 0.8 + 0.1, (1.0 - 0.69607843) * 0.8 + 0.1}, {0.48202614 * 0.8 + 0.1, (1.0 - 0.37908497) * 0.8 + 0.1}},
		{{0.24019608 * 0.8 + 0.1, (1.0 - 0.69607843) * 0.8 + 0.1}, {0.26797386 * 0.8 + 0.1, (1.0 - 0.75653595) * 0.8 + 0.1}, {0.48202614 * 0.8 + 0.1, (1.0 - 0.37908497) * 0.8 + 0.1}},
		{{0.26797386 * 0.8 + 0.1, (1.0 - 0.75653595) * 0.8 + 0.1}, {0.20588235 * 0.8 + 0.1, (1.0 - 0.88071895) * 0.8 + 0.1}, {0.48202614 * 0.8 + 0.1, (1.0 - 0.37908497) * 0.8 + 0.1}},
		{{0.20588235 * 0.8 + 0.1, (1.0 - 0.88071895) * 0.8 + 0.1}, {0.21568627 * 0.8 + 0.1, (1.0 - 0.96405229) * 0.8 + 0.1}, {0.48202614 * 0.8 + 0.1, (1.0 - 0.37908497) * 0.8 + 0.1}},
		{{0.21568627 * 0.8 + 0.1, (1.0 - 0.96405229) * 0.8 + 0.1}, {0.34640523 * 0.8 + 0.1, (1.0 - 0.97222222) * 0.8 + 0.1}, {0.48202614 * 0.8 + 0.1, (1.0 - 0.37908497) * 0.8 + 0.1}},
		{{0.34640523 * 0.8 + 0.1, (1.0 - 0.97222222) * 0.8 + 0.1}, {0.52287582 * 0.8 + 0.1, (1.0 - 0.83333333) * 0.8 + 0.1}, {0.48202614 * 0.8 + 0.1, (1.0 - 0.37908497) * 0.8 + 0.1}},
		{{0.52287582 * 0.8 + 0.1, (1.0 - 0.83333333) * 0.8 + 0.1}, {0.69934641 * 0.8 + 0.1, (1.0 - 0.94281046) * 0.8 + 0.1}, {0.48202614 * 0.8 + 0.1, (1.0 - 0.37908497) * 0.8 + 0.1}},
		{{0.69934641 * 0.8 + 0.1, (1.0 - 0.94281046) * 0.8 + 0.1}, {0.78431373 * 0.8 + 0.1, (1.0 - 0.92320261) * 0.8 + 0.1}, {0.48202614 * 0.8 + 0.1, (1.0 - 0.37908497) * 0.8 + 0.1}},
		{{0.78431373 * 0.8 + 0.1, (1.0 - 0.92320261) * 0.8 + 0.1}, {0.84313725 * 0.8 + 0.1, (1.0 - 0.82189542) * 0.8 + 0.1}, {0.48202614 * 0.8 + 0.1, (1.0 - 0.37908497) * 0.8 + 0.1}},
		{{0.84313725 * 0.8 + 0.1, (1.0 - 0.82189542) * 0.8 + 0.1}, {0.79738562 * 0.8 + 0.1, (1.0 - 0.72058824) * 0.8 + 0.1}, {0.48202614 * 0.8 + 0.1, (1.0 - 0.37908497) * 0.8 + 0.1}},
		{{0.79738562 * 0.8 + 0.1, (1.0 - 0.72058824) * 0.8 + 0.1}, {0.79738562 * 0.8 + 0.1, (1.0 - 0.63888889) * 0.8 + 0.1}, {0.48202614 * 0.8 + 0.1, (1.0 - 0.37908497) * 0.8 + 0.1}},
		{{0.79738562 * 0.8 + 0.1, (1.0 - 0.63888889) * 0.8 + 0.1}, {0.93464052 * 0.8 + 0.1, (1.0 - 0.53921569) * 0.8 + 0.1}, {0.48202614 * 0.8 + 0.1, (1.0 - 0.37908497) * 0.8 + 0.1}},
		{{0.93464052 * 0.8 + 0.1, (1.0 - 0.53921569) * 0.8 + 0.1}, {0.92647059 * 0.8 + 0.1, (1.0 - 0.38562092) * 0.8 + 0.1}, {0.48202614 * 0.8 + 0.1, (1.0 - 0.37908497) * 0.8 + 0.1}},
		{{0.92647059 * 0.8 + 0.1, (1.0 - 0.38562092) * 0.8 + 0.1}, {0.85130719 * 0.8 + 0.1, (1.0 - 0.31535948) * 0.8 + 0.1}, {0.48202614 * 0.8 + 0.1, (1.0 - 0.37908497) * 0.8 + 0.1}},
		{{0.85130719 * 0.8 + 0.1, (1.0 - 0.31535948) * 0.8 + 0.1}, {0.86274510 * 0.8 + 0.1, (1.0 - 0.11437908) * 0.8 + 0.1}, {0.48202614 * 0.8 + 0.1, (1.0 - 0.37908497) * 0.8 + 0.1}},
		{{0.86274510 * 0.8 + 0.1, (1.0 - 0.11437908) * 0.8 + 0.1}, {0.81699346 * 0.8 + 0.1, (1.0 - 0.07843137) * 0.8 + 0.1}, {0.48202614 * 0.8 + 0.1, (1.0 - 0.37908497) * 0.8 + 0.1}},
		{{0.81699346 * 0.8 + 0.1, (1.0 - 0.07843137) * 0.8 + 0.1}, {0.66339869 * 0.8 + 0.1, (1.0 - 0.10620915) * 0.8 + 0.1}, {0.48202614 * 0.8 + 0.1, (1.0 - 0.37908497) * 0.8 + 0.1}},
		{{0.66339869 * 0.8 + 0.1, (1.0 - 0.10620915) * 0.8 + 0.1}, {0.54248366 * 0.8 + 0.1, (1.0 - 0.02614379) * 0.8 + 0.1}, {0.48202614 * 0.8 + 0.1, (1.0 - 0.37908497) * 0.8 + 0.1}},
	};
	//
	for (const auto& triangle : polygon_hull)
	{
		if (Intersect(ta, tb, get<0>(triangle), get<1>(triangle), get<2>(triangle)) <= IntersectStatus::INTERSECTING)
		{
			return true;
		}
	}
	//
	// dLog("Segment((%f, %f), (%f, %f))\n", ta.x, ta.y, tb.x, tb.y);
	//
	return false;
}
