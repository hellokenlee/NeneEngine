/*Copyright reserved by KenLee@2020 ken4000kl@gmail.com*/
#include <optional>
#include <iostream>
#include "NeneEngine/Debug.h"
#include "LappedTexturePatch.h"

using namespace std;

#define PRECISION 1000000
#define TEXTURE_PASTING_SCALE 1.0f
#define COVERAGE_TEXTURE_SIZE 512
#define COVERAGE_ALPHA_THRESHOLD 10

#define IA(f) (f * 3 + 0)
#define IB(f) (f * 3 + 1)
#define IC(f) (f * 3 + 2)
#define ADJ_SHARE_I0(f, offset) (f * 3 + ((0 + offset) % 3))
#define ADJ_SHARE_I1(f, offset) (f * 3 + ((1 + offset) % 3))
#define ADJ_DIAGO_I2(f, offset) (f * 3 + ((2 + offset) % 3))


NNUInt LappedTexturePatch::AddSourceFaceToPatch(const NNUInt& sface)
{
	//
	m_candidate_faces.erase(sface);
	//
	m_coverage_faces.push_back(sface);
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

LappedTexturePatch::LappedTexturePatch(const std::vector<NNUInt>& indices, const std::vector<Vertex>& vertices, std::set<NNUInt>& faces):
	m_source_indices(indices), m_source_vertices(vertices), m_candidate_faces(faces), m_is_grown(false), m_is_optimazed(false)
{
	// 初始面
	// NNUInt face = rand() % NNUInt(candidate_faces.size());
	NNUInt sface = *(m_candidate_faces.begin());
	//
	NNUInt pface = AddSourceFaceToPatch(sface);
	//
	const NNUInt pia = m_patch_indices[IA(pface)];
	const NNUInt pib = m_patch_indices[IB(pface)];
	const NNUInt pic = m_patch_indices[IC(pface)];
	
	// 选择面内一个点
	// float b0 = float(rand() % PRECISION) / float(PRECISION);
	// float b1 = float(rand() % PRECISION) / float(PRECISION);
	// float b2 = float(rand() % PRECISION) / float(PRECISION);
	// NNVec3 barycentric(b0, b1, b2);
	// NNVec3 m_center_position = Barycentric2Cartesian(vertices[ia].m_position, vertices[ib].m_position, vertices[ic].m_position, barycentric);
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
	std::vector<NNFloat> vertices(m_coverage_faces.size() * 3 * 5);
	for (size_t fid = 0; fid < m_coverage_faces.size(); ++fid)
	{
		const NNUInt sface = m_coverage_faces[fid];
		for (NNUInt vid = 0; vid < 3; ++vid)
		{
			const NNUInt si = m_source_indices[sface * 3 + vid];
			const Vertex& sv = m_source_vertices[si];
			//
			vertices[(fid * 15) + (vid * 5) + 0] = sv.m_texcoord.x;
			vertices[(fid * 15) + (vid * 5) + 1] = sv.m_texcoord.y;
			//
			vertices[(fid * 15) + (vid * 5) + 2] = float(sface);
			//
			const NNUInt pi = m_source_to_patch_index[si];
			const Vertex pv = m_patch_vertices[pi];
			vertices[(fid * 15) + (vid * 5) + 3] = pv.m_texcoord.x;
			vertices[(fid * 15) + (vid * 5) + 4] = pv.m_texcoord.y;
		}
	}
	m_patch_coverage_mesh = Shape::Create(vertices, NNVertexFormat::POSITION_TEXTURE);
	dLog("[Patch] Regenerate coverage mesh for patch.\n");
}

void LappedTexturePatch::Grow()
{
	//
	if (m_is_grown)
	{
		dLog("[Patch] The patch is grown.\n");
		return;
	}
	//
	optional<Adjacency> adjaceny = FindNearestAdjacentFace();
	//
	if (not adjaceny.has_value())
	{
		//
		m_is_grown = true;
		m_patch_rendering_mesh = Mesh::Create(m_patch_vertices, m_patch_indices, {});
		dLog("[Patch] No adjacency found for this patch.\n");
		//
		GenerateCoverageMesh();
	}
	//
	else
	{
		// 加入邻接三角形
		NNUInt pface = AddSourceFaceToPatch(adjaceny->aface);

		// 计算非共边顶点的纹理坐标
		const NNUInt s_si0 = m_source_indices[ADJ_SHARE_I0(adjaceny->aface, adjaceny->acase)];
		const NNUInt s_si1 = m_source_indices[ADJ_SHARE_I1(adjaceny->aface, adjaceny->acase)];
		const NNUInt d_si2 = m_source_indices[ADJ_DIAGO_I2(adjaceny->aface, adjaceny->acase)];

		const NNUInt s_pi0 = m_source_to_patch_index[s_si0];
		const NNUInt s_pi1 = m_source_to_patch_index[s_si1];
		const NNUInt d_pi2 = m_source_to_patch_index[d_si2];
		//
		NNVec3 p0 = m_patch_vertices[s_pi0].m_position;
		NNVec3 p1 = m_patch_vertices[s_pi1].m_position;
		NNVec3 p2 = m_patch_vertices[d_pi2].m_position;
		//
		NNVec2 t0 = m_patch_vertices[s_pi0].m_texcoord;
		NNVec2 t1 = m_patch_vertices[s_pi1].m_texcoord;
		//
		NNVec3 n0 = m_patch_vertices[s_pi0].m_normal;
		NNVec3 n1 = m_patch_vertices[s_pi1].m_normal;
		NNVec3 n2 = m_patch_vertices[d_pi2].m_normal;
		//
		NNVec3 nf = NNNormalize((n0 + n1 + n2) / 3.0f);
		NNVec2 t2 = SimilarTriangle3DTo2D(p0, p1, p2, nf, t0, t1);
		//
		assert(m_patch_vertices[d_pi2].m_texcoord.x == 0.0f and m_patch_vertices[d_pi2].m_texcoord.y == 0.0f);
		//
		m_patch_vertices[d_pi2].m_texcoord = t2;
		//
		dLog("[Patch] {%f, %f}", t2.x, t2.y);
		dLog("[Patch] Grow patch with %dth face: %d; (%d %d %d)", pface, adjaceny->aface, s_si0, s_si1, d_si2);
		// 检查是否同时覆盖掉了其他面
		CheckAndAddCoveredFaceToPatch(*adjaceny);

		//
		m_patch_rendering_mesh = Mesh::Create(m_patch_vertices, m_patch_indices, {});
	}
	
}

void LappedTexturePatch::CheckAndAddCoveredFaceToPatch(const Adjacency& adjacency)
{
	std::vector<NNUInt> face2add;
	for (NNUInt sface : m_candidate_faces)
	{
		//
		const NNUInt sia = m_source_indices[IA(sface)];
		const NNUInt sib = m_source_indices[IB(sface)];
		const NNUInt sic = m_source_indices[IC(sface)];
		//
		bool has_ia = m_source_to_patch_index.find(sia) != m_source_to_patch_index.end();
		bool has_ib = m_source_to_patch_index.find(sib) != m_source_to_patch_index.end();
		bool has_ic = m_source_to_patch_index.find(sic) != m_source_to_patch_index.end();
		// 三个顶点都被覆盖了
		if (has_ia && has_ib && has_ic)
		{
			face2add.push_back(sface);
		}
	}
	for (NNUInt sface : face2add)
	{
		const NNUInt pface = AddSourceFaceToPatch(sface);
		const NNUInt pia = m_patch_indices[IA(pface)];
		const NNUInt pib = m_patch_indices[IB(pface)];
		const NNUInt pic = m_patch_indices[IC(pface)];
		//
		NNVec3 pa = m_patch_vertices[pia].m_position;
		NNVec3 pb = m_patch_vertices[pib].m_position;
		NNVec3 pc = m_patch_vertices[pic].m_position;
		NNVec2 ta = m_patch_vertices[pia].m_texcoord;
		NNVec2 tb = m_patch_vertices[pib].m_texcoord;
		NNVec2 tc = m_patch_vertices[pic].m_texcoord;
		//
		NNVec3 tab(tb.x - ta.x, tb.y - ta.y, 0.0f);
		NNVec3 tbc(tc.x - tb.x, tc.y - tb.y, 0.0f);
		NNVec3 tnormal = glm::cross(tab, tbc);
		//
		dLog("[Patch] Grow patch with %dth face: %d deu to merge.", pface, sface);
		//
		if (tnormal.z < 0.0f)
		{
			//
			NNVec3 pna = m_patch_vertices[pia].m_normal;
			NNVec3 pnb = m_patch_vertices[pib].m_normal;
			NNVec3 pnc = m_patch_vertices[pic].m_normal;
			NNVec3 pnf = NNNormalize((pna + pnb + pnc) / 3.0f);
			//
			const NNUInt d_si2 = m_source_indices[ADJ_DIAGO_I2(adjacency.aface, adjacency.acase)];
			const NNUInt d_pi2 = m_source_to_patch_index[d_si2];
			//
			NNVec2 d_pt;
			//
			if (pia != d_pi2 and pib != d_pi2 and pic != d_pi2)
			{
				assert(false);
			}
			else if (pia == d_pi2)
			{
				m_patch_vertices[pia].m_texcoord = SimilarTriangle3DTo2D(pb, pc, pa, pnf, tb, tc);
				d_pt = m_patch_vertices[pia].m_texcoord;
			}
			else if (pib == d_pi2)
			{
				m_patch_vertices[pib].m_texcoord = SimilarTriangle3DTo2D(pc, pa, pb, pnf, tc, ta);
				d_pt = m_patch_vertices[pia].m_texcoord;
			}
			else if (pic == d_pi2)
			{
				m_patch_vertices[pic].m_texcoord = SimilarTriangle3DTo2D(pa, pb, pc, pnf, ta, tb);
				d_pt = m_patch_vertices[pia].m_texcoord;
			}
			dLog("[Patch] {%f, %f}", d_pt.x, d_pt.y);
			dLog("[Patch] Adjust face: %d", sface);
		}
	}
}

optional<Adjacency> LappedTexturePatch::FindNearestAdjacentFace()
{
	//
	NNFloat min_dis = 3.40281e+038f;
	optional<Adjacency> min_dis_adj = nullopt;
	//
	for (NNUInt sface : m_candidate_faces)
	{
		//
		const NNUInt sia = m_source_indices[IA(sface)];
		const NNUInt sib = m_source_indices[IB(sface)];
		const NNUInt sic = m_source_indices[IC(sface)];
		//
		bool share_ia = m_source_to_patch_index.find(sia) != m_source_to_patch_index.end();
		bool share_ib = m_source_to_patch_index.find(sib) != m_source_to_patch_index.end();
		bool share_ic = m_source_to_patch_index.find(sic) != m_source_to_patch_index.end();
		//
		optional<Adjacency> adjaceny = nullopt;
		//
		if (share_ia and share_ib and share_ic)
		{
			assert(false);
		}
		// 当前三角形边 AB 是公共边
		else if (share_ia and share_ib)
		{
			adjaceny = Adjacency{ sface, AdjacencyCase::AB };
		}
		// 当前三角形边 BC 是公共边
		else if (share_ib and share_ic)
		{
			adjaceny = Adjacency{ sface, AdjacencyCase::BC };
		}
		// 当前三角形边 CA 是公共边
		else if (share_ic and share_ia)
		{
			adjaceny = Adjacency{ sface, AdjacencyCase::CA };
		}
		//
		if (adjaceny.has_value() and IsValidAdjacency(*adjaceny))
		{
			//
			const NNVec3 pa = m_source_vertices[sia].m_position;
			const NNVec3 pb = m_source_vertices[sib].m_position;
			const NNVec3 pc = m_source_vertices[sic].m_position;
			//
			const NNVec3 center_position = (pa + pb + pc) / 3.0f;

			NNFloat dis = glm::distance(center_position, m_center_position);

			if (dis < min_dis)
			{
				min_dis = dis;
				min_dis_adj = adjaceny;
			}
		}
	}
	//
	return min_dis_adj;
}

bool LappedTexturePatch::IsValidAdjacency(const Adjacency& adjcency)
{
	// 检查两个面的曲率是否过大

	
	// 检查是否在补丁块的区域中
	const NNUInt share_si0 = m_source_indices[ADJ_SHARE_I0(adjcency.aface, adjcency.acase)];
	const NNUInt share_si1 = m_source_indices[ADJ_SHARE_I1(adjcency.aface, adjcency.acase)];
	const NNUInt share_pi0 = m_source_to_patch_index[share_si0];
	const NNUInt share_pi1 = m_source_to_patch_index[share_si1];
	//
	const NNVec2 ta = m_patch_vertices[share_pi0].m_texcoord;
	const NNVec2 tb = m_patch_vertices[share_pi1].m_texcoord;
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
