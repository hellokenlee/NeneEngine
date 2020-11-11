/*Copyright reserved by KenLee@2020 ken4000kl@gmail.com*/

#include "NeneEngine/Debug.h"
#include "LappedTextureMesh.h"

using namespace std;

#define COVERAGE_TEXTURE_SIZE 4096
#define COVERAGE_ALPHA_THRESHOLD 10


LappedTextureMesh::~LappedTextureMesh() 
{}

LappedTextureMesh::LappedTextureMesh(std::shared_ptr<StaticMesh> static_mesh):
	m_source_mesh(*static_mesh), m_need_to_update_coverage(false)
{
	//
	m_debug_quad = Geometry::CreateQuad();
	//
	m_patch_texture = Texture2D::Create("Resource/Texture/splotch_checkboard.png");
	//
	m_patch_debug_shader = Shader::Create("Resource/Shader/GLSL/2DColor.vert", "Resource/Shader/GLSL/2DColor.frag", NNVertexFormat::POSITION_NORMAL_TEXTURE);
	m_texture_debug_shader = Shader::Create("Resource/Shader/GLSL/2DTexture.vert", "Resource/Shader/GLSL/2DTexture.frag", NNVertexFormat::POSITION_NORMAL_TEXTURE);
	m_patch_rendering_shader = Shader::Create("Resource/Shader/GLSL/Patch.vert", "Resource/Shader/GLSL/Patch.frag", NNVertexFormat::POSITION_NORMAL_TEXTURE);
	//
	m_coverage_shader = Shader::Create("Resource/Shader/GLSL/Coverage.vert", "Resource/Shader/GLSL/Coverage.frag", NNVertexFormat::POSITION_TEXTURE);
	m_coverage_rtt = RenderTarget::Create(4096, 4096, 1);
	//
	const std::vector<NNUInt>& indices = m_source_mesh.GetMeshes()[0]->GetIndexData();
	//
	for (NNUInt f = 0; f < NNUInt(indices.size()) / 3; ++f)
	{
		m_candidate_faces.insert(f);
	}
	//
	BuildFaceAdjacency();
}

NNUInt LappedTextureMesh::AddPatch()
{
	//
	const std::shared_ptr<Mesh> mesh = m_source_mesh.GetMeshes()[0];
	//
	LappedTexturePatch patch(mesh->GetIndexData(), mesh->GetVertexData(), m_source_face_adjacencies, m_candidate_faces);
	//
	m_patches.emplace_back(patch);
	//
	return NNUInt(m_patches.size() - 1);
}

void LappedTextureMesh::Draw()
{
	m_patch_rendering_shader->Use();
	m_patch_texture->Use();
	for (auto it = m_patches.rbegin(); it != m_patches.rend(); ++it)
	{
		it->Draw();
	}
}

void LappedTextureMesh::DrawDebug(const NNUInt& i)
{
	{
		m_patch_texture->Use(0);
		m_texture_debug_shader->Use();
		m_debug_quad->Draw();
	}
	
	m_patch_debug_shader->Use();
	if(i < m_patches.size())
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		m_patches[i].Draw();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void LappedTextureMesh::DrawAndCalcFaceCoverage()
{
	//
	if (not m_need_to_update_coverage)
	{
		return;
	}
	m_need_to_update_coverage = false;
	//
	m_coverage_rtt->Begin();
	{
		Utils::Clear();
		m_coverage_shader->Use();
		m_patch_texture->Use(0);
		for (const auto& patch : m_patches)
		{
			patch.DrawCoverage();
		}
	}
	m_coverage_rtt->End();
	
	//
	shared_ptr<NNByte[]> bits = m_coverage_rtt->GetColorTex(0)->GetPixelData();
	assert(bits != nullptr);
	//
	std::set<NNUInt> readd_faces;
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
	//
	for (const auto face : readd_faces)
	{
		m_candidate_faces.insert(face);
		dLog("[Coverage] Re-add candidate face: %d", face);
	}
	//
	m_coverage_rtt->GetColorTex(0)->SavePixelData("coverage.png");
}

void LappedTextureMesh::BuildFaceAdjacency()
{
	//
	const std::shared_ptr<Mesh> mesh = m_source_mesh.GetMeshes()[0];
	const auto& indices = mesh->GetIndexData();
	const auto& vertices = mesh->GetVertexData();
	//
	NNUInt face_num = NNUInt(indices.size() / 3);
	//
	m_source_face_adjacencies.clear();
	m_source_face_adjacencies.resize(face_num);
	//
	for (NNUInt src_face = 0; src_face < face_num; ++src_face)
	{
		for (NNUInt dst_face = src_face + 1; dst_face < face_num; ++dst_face)
		{
			optional<FaceAdjacency> adj = CalcAdjacentEdge(indices, vertices, src_face, dst_face);
			if (adj.has_value())
			{
				m_source_face_adjacencies[src_face][dst_face] = FaceAdjacency{ src_face, dst_face, adj->src_edge, adj->dst_edge };
				m_source_face_adjacencies[dst_face][src_face] = FaceAdjacency{ dst_face, src_face, adj->dst_edge, adj->src_edge };
			}
		}
	}
	//
}
