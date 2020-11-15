/*Copyright reserved by KenLee@2020 ken4000kl@gmail.com*/
#include <optional>
#include <iostream>
#include "NeneEngine/Debug.h"
#include "LappedTexturePatch.h"

using namespace std;

#define PRECISION 1000000
#define TEXTURE_PASTING_SCALE 1.0f

#define IA(f) (f * 3 + 0)
#define IB(f) (f * 3 + 1)
#define IC(f) (f * 3 + 2)


NNUInt LappedTexturePatch::AddSourceFaceToPatch(const NNUInt& sface)
{
	assert(m_candidate_faces.find(sface) != m_candidate_faces.end());
	//
	m_candidate_faces.erase(sface);
	//
	m_source_coverage_faces.insert(sface);
	//
	for (size_t si_it = IA(sface); si_it <= IC(sface); ++si_it)
	{
		//
		NNUInt si = m_source_indices[si_it];
		//
		NNUInt pi;
		auto pi_it = m_source_to_patch_index.find(si);
		if (pi_it != m_source_to_patch_index.end())
		{
			pi = pi_it->second;
		}
		else
		{
			pi = NNUInt(m_patch_vertices.size());
			m_source_to_patch_index.insert(make_pair(si, pi));
			Vertex pvertex = m_source_vertices[si];
			pvertex.m_texcoord *= 0.0f;
			m_patch_vertices.emplace_back(pvertex);
		}
		m_patch_indices.push_back(pi);
	}
	//
	return NNUInt((m_patch_indices.size() / 3) - 1);
}

LappedTexturePatch::~LappedTexturePatch()
{}

LappedTexturePatch::LappedTexturePatch(const std::vector<NNUInt>& indices, const std::vector<Vertex>& vertices, const std::vector<std::unordered_map<NNUInt, FaceAdjacency>>& face_adjs, std::unordered_set<NNUInt>& faces):
	m_source_indices(indices), m_source_vertices(vertices), m_candidate_faces(faces), m_source_face_adjacencies(face_adjs), m_is_grown(false)
{
	// 初始面
	//int random_pos = rand() % int(m_candidate_faces.size());
	//auto random_it = std::next(std::begin(m_candidate_faces), random_pos);
	//NNUInt sface = *random_it;
	//NNUInt sface = *(m_candidate_faces.begin());
	NNUInt sface = 1525;
	//
	NNUInt pface = AddSourceFaceToPatch(sface);
	//
	const NNUInt pia = m_patch_indices[IA(pface)];
	const NNUInt pib = m_patch_indices[IB(pface)];
	const NNUInt pic = m_patch_indices[IC(pface)];
	
	// 选择面内一个点
	// m_center_position = RandomPositionInTriangle(m_patch_vertices[pia].m_position, m_patch_vertices[pib].m_position, m_patch_vertices[pic].m_position);
	m_center_position = (m_patch_vertices[pia].m_position + m_patch_vertices[pib].m_position + m_patch_vertices[pic].m_position) / 3.0f;

	// 选择的点对准切线空间的原点
	NNVec3 tagent, bitangent, normal;
	normal = NNNormalize((m_patch_vertices[pia].m_normal + m_patch_vertices[pib].m_normal + m_patch_vertices[pic].m_normal) / 3.0f);
	CalcTangentAndBitangent(m_patch_vertices[pia].m_position, m_patch_vertices[pib].m_position, m_patch_vertices[pic].m_position, normal, tagent, bitangent);

	// 计算纹理坐标 注意最后一个分量应为零
	NNMat3 tbn(tagent, bitangent, normal);
	NNMat3 tbn_inversed = glm::inverse(tbn);
	NNVec3 center_in_tbn = tbn_inversed * m_center_position;
	NNVec3 vertex_a_in_tbn = tbn_inversed * m_patch_vertices[pia].m_position - center_in_tbn;
	NNVec3 vertex_b_in_tbn = tbn_inversed * m_patch_vertices[pib].m_position - center_in_tbn;
	NNVec3 vertex_c_in_tbn = tbn_inversed * m_patch_vertices[pic].m_position - center_in_tbn;
	//
	m_patch_vertices[pia].m_texcoord = (NNVec2(vertex_a_in_tbn.x, vertex_a_in_tbn.y) * TEXTURE_PASTING_SCALE) + NNVec2(0.5f, 0.5f);
	m_patch_vertices[pib].m_texcoord = (NNVec2(vertex_b_in_tbn.x, vertex_b_in_tbn.y) * TEXTURE_PASTING_SCALE) + NNVec2(0.5f, 0.5f);
	m_patch_vertices[pic].m_texcoord = (NNVec2(vertex_c_in_tbn.x, vertex_c_in_tbn.y) * TEXTURE_PASTING_SCALE) + NNVec2(0.5f, 0.5f);
	//
	m_patch_rendering_mesh = Mesh::Create(m_patch_vertices, m_patch_indices, {});
	//
	dLog("[Patch] Init add face: %d; Remain: %zd faces; (%d, %d, %d)", sface, m_candidate_faces.size(), m_source_indices[IA(sface)], m_source_indices[IB(sface)], m_source_indices[IC(sface)]);
}

void LappedTexturePatch::Draw() const
{
	m_patch_rendering_mesh->Draw();
}

void LappedTexturePatch::DrawCoverage() const
{
	//
	if (m_patch_coverage_mesh == nullptr)
	{
		return;
	}
	//
	m_patch_coverage_mesh->Draw();
}

void LappedTexturePatch::GenerateCoverageMesh()
{
	// vertices:
	//		vec3: (SrcTexCoordU, SrcTexCoordV, FaceIndex)
	//		vec2: (PatchTexCoordU, PatchTexCoordV)
	std::vector<NNFloat> vertices(m_source_coverage_faces.size() * 3 * 5);
	size_t index = 0;
	for (NNUInt sface : m_source_coverage_faces)
	{
		for (NNUInt vid = 0; vid < 3; ++vid)
		{
			const NNUInt si = m_source_indices[sface * 3 + vid];
			const Vertex& sv = m_source_vertices[si];
			//
			vertices[(index * 15) + (vid * 5) + 0] = sv.m_texcoord.x;
			vertices[(index * 15) + (vid * 5) + 1] = sv.m_texcoord.y;
			//
			vertices[(index * 15) + (vid * 5) + 2] = float(sface);
			//
			const NNUInt pi = m_source_to_patch_index[si];
			const Vertex pv = m_patch_vertices[pi];
			vertices[(index * 15) + (vid * 5) + 3] = pv.m_texcoord.x;
			vertices[(index * 15) + (vid * 5) + 4] = pv.m_texcoord.y;
		}
		++index;
	}
	m_patch_coverage_mesh = Shape::Create(vertices, NNVertexFormat::POSITION_TEXTURE);
	dLog("[Patch] Regenerate coverage mesh for patch.\n");
}

void LappedTexturePatch::Grow()
{
	if (m_is_grown)
	{
		dLog("[Patch] The patch is grown.\n");
		return;
	}
	optional<NNUInt> pface = AddNearestAdjacentFaceToPatch();
	//
	if (not pface.has_value())
	{
		//
		m_is_grown = true;
		m_patch_rendering_mesh = Mesh::Create(m_patch_vertices, m_patch_indices, {});
		dLog("[Patch] No adjacency found for this patch. Patch face num: %zd; Remain face num: %zd\n", m_source_coverage_faces.size(), m_candidate_faces.size());
		//
		GenerateCoverageMesh();
	}
	else
	{
		m_patch_rendering_mesh = Mesh::Create(m_patch_vertices, m_patch_indices, {});
	}
}

optional<NNUInt> LappedTexturePatch::AddNearestAdjacentFaceToPatch()
{
	// 找出当前区域的所有相邻
	std::vector<FaceAdjacency> neighbors;
	for (NNUInt src_face : m_source_coverage_faces)
	{
		for (const auto it : m_source_face_adjacencies[src_face])
		{
			NNUInt dst_face = it.first;
			if (m_candidate_faces.find(dst_face) != m_candidate_faces.end())
			{
				neighbors.push_back(it.second);
			}
		}
	}
	// 找出距离中心最小的合法相邻
	NNFloat min_dis = 3.40281e+038f;
	optional<FaceAdjacency> min_dis_adj = nullopt;
	//
	for (auto& adj : neighbors)
	{
		const NNVec3 pa = m_source_vertices[m_source_indices[IA(adj.dst_face)]].m_position;
		const NNVec3 pb = m_source_vertices[m_source_indices[IB(adj.dst_face)]].m_position;
		const NNVec3 pc = m_source_vertices[m_source_indices[IC(adj.dst_face)]].m_position;
		const NNVec3 center_position = (pa + pb + pc) / 3.0f;
		NNFloat curr_distance = glm::distance(center_position, m_center_position);
		if (curr_distance < min_dis and IsValidAdjacency(adj))
		{
			min_dis = curr_distance;
			min_dis_adj = adj;
		}
	}

	// 计算或复制这个相邻面的纹理坐标
	if (min_dis_adj.has_value())
	{
		// 加到当前区域中
		NNUInt pface = AddSourceFaceToPatch(min_dis_adj->dst_face);
		// 拷贝相邻边的顶点的纹理坐标
		CopyPatchAdjacencyVertexTexcoord(*min_dis_adj, false);
		//
		bool need_to_calc_texcoord = true;
		for (const auto& kv : m_source_face_adjacencies[min_dis_adj->dst_face])
		{
			const NNUInt& other_face = kv.first;
			if (other_face != min_dis_adj->src_face and m_source_coverage_faces.find(other_face) != m_source_coverage_faces.end())
			{
				const FaceAdjacency& other_adj = kv.second;
				CopyPatchAdjacencyVertexTexcoord(other_adj, true);
				need_to_calc_texcoord = false;
				dLog("[Patch] Grow with face that all vertices is in patch! (%d: %d, %d)", min_dis_adj->dst_face, min_dis_adj->src_face, other_face);
				break;
			}
		}
		// 计算非相邻边顶点的纹理坐标
		if (need_to_calc_texcoord)
		{
			//
			const NNUInt dst_share_pi0 = m_source_to_patch_index[m_source_indices[DST_ADJ_SHARE_I0(min_dis_adj->dst_face, min_dis_adj->dst_edge)]];
			const NNUInt dst_share_pi1 = m_source_to_patch_index[m_source_indices[DST_ADJ_SHARE_I1(min_dis_adj->dst_face, min_dis_adj->dst_edge)]];
			const NNUInt dst_diago_pi2 = m_source_to_patch_index[m_source_indices[DST_ADJ_DIAGO_I2(min_dis_adj->dst_face, min_dis_adj->dst_edge)]];
			//
			NNVec3 p0 = m_patch_vertices[dst_share_pi0].m_position;
			NNVec3 p1 = m_patch_vertices[dst_share_pi1].m_position;
			NNVec3 p2 = m_patch_vertices[dst_diago_pi2].m_position;
			//
			NNVec2 t0 = m_patch_vertices[dst_share_pi0].m_texcoord;
			NNVec2 t1 = m_patch_vertices[dst_share_pi1].m_texcoord;
			//
			NNVec3 n0 = m_patch_vertices[dst_share_pi0].m_normal;
			NNVec3 n1 = m_patch_vertices[dst_share_pi1].m_normal;
			NNVec3 n2 = m_patch_vertices[dst_diago_pi2].m_normal;
			//
			NNVec3 nf = NNNormalize((n0 + n1 + n2) / 3.0f);
			NNVec2 t2 = SimilarTriangle3DTo2D(p0, p1, p2, nf, t0, t1);
			m_patch_vertices[dst_diago_pi2].m_texcoord = t2;
		}
		dLog("[Patch] Grow add %zdth face: %d;", m_source_coverage_faces.size(), min_dis_adj->dst_face);
		return pface;
	}

	return nullopt;
}

void LappedTexturePatch::CopyPatchAdjacencyVertexTexcoord(const FaceAdjacency& adj, const bool& dst_to_src)
{
	//
	const NNUInt src_share_si0 = m_source_indices[SRC_ADJ_SHARE_I0(adj.src_face, adj.src_edge)];
	const NNUInt src_share_si1 = m_source_indices[SRC_ADJ_SHARE_I1(adj.src_face, adj.src_edge)];
	const NNUInt dst_share_si0 = m_source_indices[DST_ADJ_SHARE_I0(adj.dst_face, adj.dst_edge)];
	const NNUInt dst_share_si1 = m_source_indices[DST_ADJ_SHARE_I1(adj.dst_face, adj.dst_edge)];
	//
	const NNUInt src_share_pi0 = m_source_to_patch_index[src_share_si0];
	const NNUInt src_share_pi1 = m_source_to_patch_index[src_share_si1];
	const NNUInt dst_share_pi0 = m_source_to_patch_index[dst_share_si0];
	const NNUInt dst_share_pi1 = m_source_to_patch_index[dst_share_si1];
	//
	if (dst_to_src)
	{
		m_patch_vertices[src_share_pi0].m_texcoord = m_patch_vertices[dst_share_pi0].m_texcoord;
		m_patch_vertices[src_share_pi1].m_texcoord = m_patch_vertices[dst_share_pi1].m_texcoord;
	}
	else
	{
		m_patch_vertices[dst_share_pi0].m_texcoord = m_patch_vertices[src_share_pi0].m_texcoord;
		m_patch_vertices[dst_share_pi1].m_texcoord = m_patch_vertices[src_share_pi1].m_texcoord;
	}
}

bool LappedTexturePatch::IsValidAdjacency(const FaceAdjacency& adj)
{
	//
	const NNUInt src_share_si0 = m_source_indices[SRC_ADJ_SHARE_I0(adj.src_face, adj.src_edge)];
	const NNUInt src_share_si1 = m_source_indices[SRC_ADJ_SHARE_I1(adj.src_face, adj.src_edge)];
	const NNUInt dst_diago_si2 = m_source_indices[DST_ADJ_DIAGO_I2(adj.dst_face, adj.dst_edge)];
	//
	const NNVec2 t0 = m_patch_vertices[m_source_to_patch_index[src_share_si0]].m_texcoord;
	const NNVec2 t1 = m_patch_vertices[m_source_to_patch_index[src_share_si1]].m_texcoord;
	//
	if (not IsInsidePatchHull(t0, t1))
	{
		return false;
	}
	//
	const NNVec3 p0 = m_source_vertices[src_share_si0].m_position;
	const NNVec3 p1 = m_source_vertices[src_share_si1].m_position;
	const NNVec3 p2 = m_source_vertices[dst_diago_si2].m_position;
	const NNVec3 n0 = m_source_vertices[src_share_si0].m_normal;
	const NNVec3 n1 = m_source_vertices[src_share_si1].m_normal;
	const NNVec3 n2 = m_source_vertices[dst_diago_si2].m_normal;
	//
	const NNVec3 nf = NNNormalize((n0 + n1 + n2) / 3.0f);
	//
	const NNVec2 t2 = SimilarTriangle3DTo2D(p0, p1, p2, nf, t0, t1);
	//
	NNVec3 t01(t1.x - t0.x, t1.y - t0.y, 0.0f);
	NNVec3 t12(t2.x - t1.x, t2.y - t1.y, 0.0f);
	//
	if (t2.x < -0.1f or t2.x > 1.1f or t2.y < -0.1f or t2.y > 1.1f)
	{
		return false;
	}
	//
	if (glm::cross(t01, t12).z < 0.0f)
	{
		return false;
	}
	//
	NNVec3 na = m_source_vertices[m_source_indices[IA(adj.src_face)]].m_normal;
	NNVec3 nb = m_source_vertices[m_source_indices[IB(adj.src_face)]].m_normal;
	NNVec3 nc = m_source_vertices[m_source_indices[IC(adj.src_face)]].m_normal;
	//
	NNVec3 nf_origin = NNNormalize((na + nb + nc) / 3.0f);
	if(NNDot(nf, nf_origin) < 0.95f)
	{
		return false;
	}
	dLog("[Patch] Curvature: %f", NNDot(nf, nf_origin));
	//
	return true;
}

bool LappedTexturePatch::IsInsidePatchHull(const NNVec2& ta, const NNVec2& tb)
{
	//
	assert(ta.x != 0.0f or ta.y != 0.0f);
	assert(tb.x != 0.0f or tb.y != 0.0f);
	//
	assert(ta.x != 0.0f or ta.y != 0.0f);
	assert(tb.x != 0.0f or tb.y != 0.0f);
	// 检查是否在补丁块的区域中
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
