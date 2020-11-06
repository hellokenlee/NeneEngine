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

#define A(f) (f * 3 + 0)
#define B(f) (f * 3 + 1)
#define C(f) (f * 3 + 2)


LappedTexturePatch::~LappedTexturePatch()
{}

LappedTexturePatch::LappedTexturePatch(const std::vector<NNUInt>& indices, const std::vector<Vertex> vertices, std::set<NNUInt>& faces):
	m_source_indices(indices), m_source_vertices(vertices), m_candidate_faces(faces), m_is_grown(false), m_is_optimazed(false), m_patch_faces_num(0)
{
	// 初始面
	// NNUInt face = rand() % NNUInt(candidate_faces.size());
	NNUInt face = *(m_candidate_faces.begin());
	//
	CopySourceFaceToPatch(face);
	const NNUInt pia = m_patch_vertices.size() - 3;
	const NNUInt pib = m_patch_vertices.size() - 2;
	const NNUInt pic = m_patch_vertices.size() - 1;
	
	// 选择面内一个点
	// float b0 = float(rand() % PRECISION) / float(PRECISION);
	// float b1 = float(rand() % PRECISION) / float(PRECISION);
	// float b2 = float(rand() % PRECISION) / float(PRECISION);
	// NNVec3 barycentric(b0, b1, b2);
	// NNVec3 center = Barycentric2Cartesian(vertices[ia].m_position, vertices[ib].m_position, vertices[ic].m_position, barycentric);
	m_center_position = (m_patch_vertices[pia].m_position + m_patch_vertices[pib].m_position + m_patch_vertices[pic].m_position) / 3.0f;

	// 选择的点对准切线空间的原点
	NNVec3 tagent, bitangent, normal;
	normal = NNNormalize((m_patch_vertices[pia].m_normal + m_patch_vertices[pib].m_normal + m_patch_vertices[pic].m_normal) / 3.0f);
	CalcTangentAndBitangent(m_patch_vertices[pia].m_position, m_patch_vertices[pib].m_position, m_patch_vertices[pic].m_position, normal, tagent, bitangent);

	// 计算纹理坐标 注意最后一个分量应为零
	NNMat3 tbn(tagent, bitangent, normal);
	NNMat3 tbn_inversed = glm::inverse(tbn);
	NNVec3 center_in_tbn = tbn_inversed * m_center;
	NNVec3 vertex_a_in_tbn = tbn_inversed * m_patch_vertices[pia].m_position - center_in_tbn;
	NNVec3 vertex_b_in_tbn = tbn_inversed * m_patch_vertices[pib].m_position - center_in_tbn;
	NNVec3 vertex_c_in_tbn = tbn_inversed * m_patch_vertices[pic].m_position - center_in_tbn;
	//
	m_patch_vertices[pia].m_texcoord = (NNVec2(vertex_a_in_tbn.x, vertex_a_in_tbn.y) * TEXTURE_PASTING_SCALE) + NNVec2(0.5f, 0.5f);
	m_patch_vertices[pib].m_texcoord = (NNVec2(vertex_b_in_tbn.x, vertex_b_in_tbn.y) * TEXTURE_PASTING_SCALE) + NNVec2(0.5f, 0.5f);
	m_patch_vertices[pic].m_texcoord = (NNVec2(vertex_c_in_tbn.x, vertex_c_in_tbn.y) * TEXTURE_PASTING_SCALE) + NNVec2(0.5f, 0.5f);
	//
	m_patch_mesh = Mesh::Create(m_patch_vertices);
	//
	dLog("[Patch] Init add face: %d; Remain: %zd faces;\n", face, m_candidate_faces.size());
}

void LappedTexturePatch::CopySourceFaceToPatch(const NNUInt& face)
{
	//
	m_candidate_faces.erase(face);
	//
	const NNUInt sia = m_source_indices[A(face)];
	const NNUInt sib = m_source_indices[B(face)];
	const NNUInt sic = m_source_indices[C(face)];
	//
	Vertex va = m_source_vertices[sia];
	Vertex vb = m_source_vertices[sib];
	Vertex vc = m_source_vertices[sic];
	//
	va.m_texcoord *= 0.0f;
	vb.m_texcoord *= 0.0f;
	vc.m_texcoord *= 0.0f;
	//
	m_patch_vertices.emplace_back(va);
	m_patch_source_indices.push_back(sia);
	m_patch_vertices.emplace_back(vb);
	m_patch_source_indices.push_back(sib);
	m_patch_vertices.emplace_back(vc);
	m_patch_source_indices.push_back(sic);
	//
	m_patch_faces_num += 1;
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
	optional<Adjacency> adjaceny = CopyNearestAdjacentFaceToPatch();
	//
	if (not adjaceny.has_value())
	{
		m_is_grown = true;
	}
	//
	else
	{
		// 计算非共边顶点的纹理坐标
		NNUInt s_pia = (m_patch_vertices.size() - 3 + adjaceny->adj_case) % 3;
		NNUInt s_pib = (m_patch_vertices.size() - 2 + adjaceny->adj_case) % 3;
		NNUInt d_pic = (m_patch_vertices.size() - 1 + adjaceny->adj_case) % 3;
		//
		NNVec3 pa = m_patch_vertices[s_pia].m_position;
		NNVec3 pb = m_patch_vertices[s_pib].m_position;
		NNVec3 pc = m_patch_vertices[d_pic].m_position;
		//
		NNVec2 ta = m_patch_vertices[s_pia].m_texcoord;
		NNVec2 tb = m_patch_vertices[s_pib].m_texcoord;
		//
		NNVec3 na = m_source_vertices[s_pia].m_normal;
		NNVec3 nb = m_source_vertices[s_pib].m_normal;
		NNVec3 nc = m_source_vertices[d_pic].m_normal;
		//
		NNVec3 nf = NNNormalize((na + nb + nc) / 3.0f);
		NNVec2 tc = SimilarTriangle3DTo2D(pa, pb, pc, nf, ta, tb);
		//
		m_patch_vertices[d_pic].m_texcoord = tc;
		//
		dLog("[Patch] Grow %dth face of patch.\n", m_patch_faces_num);
		//
		m_patch_mesh = Mesh::Create(m_patch_vertices);
	}
	
}

/** This function must be called before swapbufer. */
void LappedTexturePatch::CalcNotFullyCoveredFaces(std::set<NNUInt>& recover_faces)
{
	//
	if (m_is_calced_coverage)
	{
		return;
	}
	// 创建渲染资源
	if (m_shader_coverage == nullptr)
	{
		m_shader_coverage = Shader::Create("Resource/Shader/GLSL/Coverage.vert", "Resource/Shader/GLSL/Coverage.frag", POSITION);
	}
	if (m_coverage_rtt == nullptr)
	{
		m_coverage_rtt = RenderTarget::Create(COVERAGE_TEXTURE_SIZE, COVERAGE_TEXTURE_SIZE, 1);
	}

	// 创建网格资源
	if (m_coverage_mesh == nullptr)
	{
		const vector<NNUInt>& indices = m_source_mesh->GetIndexData();
		const vector<Vertex>& vertices = m_source_mesh->GetVertexData();
		vector<NNFloat> coverage_vertices;
		for (const NNUInt face : m_faces)
		{
			for (int v = 0; v < 3; ++v)
			{
				NNUInt i = indices[face * 3 + v];
				NNVec2 t = vertices[i].m_texcoord;
				coverage_vertices.push_back(t.x);
				coverage_vertices.push_back(t.y);
				coverage_vertices.push_back(float(face));
			}
		}
		m_coverage_mesh = Shape::Create(coverage_vertices, POSITION);
	}

	// 绘制覆盖情况
	m_coverage_rtt->Begin();
	{
		Utils::Clear();
		m_shader_coverage->Use();
		m_patch_tex->Use(0);
		m_coverage_mesh->Draw();
	}
	m_coverage_rtt->End();
	
	//
	m_coverage_rtt->GetColorTex(0)->SavePixelData("debug.png");
	//*
	shared_ptr<NNByte[]> bits = m_coverage_rtt->GetColorTex(0)->GetPixelData();
	assert(bits != nullptr);
	//
	std::set<NNUInt> readd_faces;
	//
	for (NNUInt y = 0; y < COVERAGE_TEXTURE_SIZE; ++y)
	{
		for (NNUInt x = 0; x < COVERAGE_TEXTURE_SIZE; ++x)
		{
			//
			NNByte b = bits[(y * COVERAGE_TEXTURE_SIZE * 4) + (x * 4) + 0];
			NNByte g = bits[(y * COVERAGE_TEXTURE_SIZE * 4) + (x * 4) + 1];
			NNByte r = bits[(y * COVERAGE_TEXTURE_SIZE * 4) + (x * 4) + 2];
			NNByte a = bits[(y * COVERAGE_TEXTURE_SIZE * 4) + (x * 4) + 3];
			//
			NNUInt face = (NNUInt(r) << 8) + NNUInt(g);
			//
			bool covered = b > COVERAGE_ALPHA_THRESHOLD;
			//
			if (not covered)
			{
				readd_faces.insert(face);
			}
		}
	}
	// Check
	{
		for (auto f : readd_faces)
		{
			assert(find(m_faces.begin(), m_faces.end(), f) != m_faces.end());
			recover_faces.insert(f);
		}
		dLog("[Patch] Re-add %zd not fully coverd faces in %zd patch faces\n", readd_faces.size(), m_faces.size());
	}
	//*/
	m_is_calced_coverage = true;
}

void LappedTexturePatch::Optimaze()
{
	for (auto&[face, phi] : m_face_phi)
	{
		cout << face <<endl << phi << endl << endl;
	}
}


void LappedTexturePatch::DrawMesh()
{
	m_patch_mesh->Draw();
}

void LappedTexturePatch::DrawFaceCoverage()
{
	
}

void LappedTexturePatch::UpdateForRendering()
{
	// Patch mesh

}

optional<Adjacency> LappedTexturePatch::FindNearestAdjacentFace()
{
	//
	NNFloat min_dis = 3.40281e+038f;
	optional<Adjacency> min_dis_adj = nullopt;
	//
	for (NNUInt index = 0; index < m_patch_source_indices.size(); ++index)
	{
		//
		NNUInt ori_f = m_patch_source_faces[index];
		//
		optional<Adjacency> adjaceny = nullopt;
		//
		for (NNUInt can_f : m_candidate_faces)
		{
			//
			const NNUInt ia = m_source_indices[A(can_f)];
			const NNUInt ib = m_source_indices[B(can_f)];
			const NNUInt ic = m_source_indices[C(can_f)];
			//
			const auto ori_f_beg = m_source_indices.begin() + (ori_f * 3);
			const auto ori_f_end = m_source_indices.begin() + (ori_f * 3 + 3);
			bool share_ia = find(ori_f_beg, ori_f_end, ia) != m_source_indices.end();
			bool share_ib = find(ori_f_beg, ori_f_end, ib) != m_source_indices.end();
			bool share_ic = find(ori_f_beg, ori_f_end, ic) != m_source_indices.end();
			// 
			if (share_ia and share_ib and share_ic)
			{
				assert(false);
			}
			// 当前三角形边 AB 是公共边
			else if (share_ia and share_ib)
			{
				adjaceny = Adjacency{ index, can_f, ia, ib, ic };
			}
			// 当前三角形边 BC 是公共边
			else if (share_ib and share_ic)
			{
				adjaceny = Adjacency{ index, can_f, ib, ic, ia };
			}
			// 当前三角形边 CA 是公共边
			else if (share_ib and share_ic)
			{
				adjaceny = Adjacency{ index, can_f, ic, ia, ib };
			}
		}
		//
		if (adjaceny.has_value() && IsValidAdjacency(*adjaceny))
		{
			//
			const NNVec3 pa = m_source_vertices[adjaceny->s_ia].m_position;
			const NNVec3 pb = m_source_vertices[adjaceny->s_ib].m_position;
			const NNVec3 pc = m_source_vertices[adjaceny->d_ic].m_position;
			//
			const NNVec3 center = (pa + pb + pc) / 3.0f;
			
			NNFloat dis = glm::distance(center, m_center);

			if (dis < min_dis)
			{
				min_dis = dis;
				min_dis_adj = adjaceny;
			}
		}
	}
	return min_dis_adj;
}

bool LappedTexturePatch::IsValidAdjacency(const Adjacency& adjcency)
{
	// 检查两个面的曲率是否过大

	
	// 检查是否在补丁块的区域中
	const NNUInt ia = adjcency.s_ia;
	const NNUInt ib = adjcency.s_ia;
	//
	const NNVec2 ta = m_patch_vertices[adjcency.patch_face_idx].m_texcoord;
	const NNVec2 tb = m_patch_vertices[ib].m_texcoord;
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
