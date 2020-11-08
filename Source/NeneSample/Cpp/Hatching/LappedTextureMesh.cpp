/*Copyright reserved by KenLee@2020 ken4000kl@gmail.com*/

#include "LappedTextureMesh.h"

LappedTextureMesh::~LappedTextureMesh() 
{}

LappedTextureMesh::LappedTextureMesh(std::shared_ptr<StaticMesh> static_mesh):
	m_source(*static_mesh)
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
	const std::vector<NNUInt>& indices = m_source.GetMeshes()[0]->GetIndexData();
	//
	for (NNUInt f = 0; f < NNUInt(indices.size()) / 3; ++f)
	{
		m_candidate_faces.insert(f);
	}
}

NNUInt LappedTextureMesh::AddPatch()
{
	//
	const std::shared_ptr<Mesh> mesh = m_source.GetMeshes()[0];
	LappedTexturePatch patch(mesh->GetIndexData(), mesh->GetVertexData(), m_candidate_faces);
	//
	m_patches.emplace_back(patch);
	//
	return NNUInt(m_patches.size() - 1);
}

void LappedTextureMesh::Draw()
{
	m_patch_rendering_shader->Use();
	m_patch_texture->Use();
	for (auto& patch : m_patches)
	{
		patch.Draw();
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
