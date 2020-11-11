/*Copyright reserved by KenLee@2020 ken4000kl@gmail.com*/

#include "NeneEngine/Debug.h"
#include "LappedTextureMesh.h"

using namespace std;

#define COVERAGE_TEXTURE_SIZE 4096
#define COVERAGE_ALPHA_THRESHOLD 10


struct FaceEdge
{
	NNUInt s_i;
	NNUInt e_i;
	NNUInt face;
	AdjacentEdge edge;
};


LappedTextureMesh::~LappedTextureMesh() 
{}

LappedTextureMesh::LappedTextureMesh(const char* filepath):
	m_source_mesh(nullptr), m_need_to_update_coverage(false)
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
	ReadOBJFileAndBuildSourceFaceAdjacencies(filepath);
	//
	const std::vector<NNUInt>& indices = m_source_mesh->GetIndexData();
	//
	for (NNUInt f = 0; f < NNUInt(indices.size()) / 3; ++f)
	{
		m_candidate_faces.insert(f);
	}
}

LappedTextureMesh::LappedTextureMesh(std::shared_ptr<Mesh> static_mesh):
	m_source_mesh(static_mesh), m_need_to_update_coverage(false)
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
	const std::vector<NNUInt>& indices = m_source_mesh->GetIndexData();
	//
	for (NNUInt f = 0; f < NNUInt(indices.size()) / 3; ++f)
	{
		m_candidate_faces.insert(f);
	}
	//
	m_source_face_adjacencies_cachepath = "FaceAdjacencies.cache";
	//
	if (not ReadSourceFaceAdjacencies())
	{
		BuildSourceFaceAdjacencies();
		WriteSourceFaceAdjacencies();
	}
}

void LappedTextureMesh::ReadOBJFileAndBuildSourceFaceAdjacencies(const char* filepath)
{
	FILE *file = fopen(filepath, "r");
	if (file == nullptr)
	{
		return;
	}
	//
	char linebuff[512];
	vector<NNVec3> normals;
	vector<NNVec3> positions;
	vector<NNVec2> texcoords;
	vector<NNUInt> positions_indices;
	vector<NNUInt> texcoords_indices;
	while (fscanf(file, "%s", linebuff) != EOF)
	{
		if (linebuff[0] == 'v' and linebuff[1] == 'n')
		{
			NNVec3 temp;
			fscanf(file, "%f %f %f", &temp.x, &temp.y, &temp.z);
			normals.emplace_back(temp);
		}
		else if (linebuff[0] == 'v' and linebuff[1] == 't')
		{
			NNVec2 temp;
			fscanf(file, "%f %f", &temp.x, &temp.y);
			texcoords.emplace_back(temp);
		}
		else if (linebuff[0] == 'v')
		{
			NNVec3 temp;
			fscanf(file, "%f %f %f", &temp.x, &temp.y, &temp.z);
			positions.emplace_back(temp);
		}
		else if (linebuff[0] == 'f')
		{
			NNUInt iav, iat, ibv, ibt, icv, ict;
			fscanf(file, "%d/%d %d/%d %d/%d", &iav, &iat, &ibv, &ibt, &icv, &ict);
			positions_indices.push_back(iav - 1);
			positions_indices.push_back(ibv - 1);
			positions_indices.push_back(icv - 1);
			texcoords_indices.push_back(iat - 1);
			texcoords_indices.push_back(ibt - 1);
			texcoords_indices.push_back(ict - 1);
		}
		else if (linebuff[0] == '#')
		{
			fgets(linebuff, 512, file);
		}
		else
		{
			fgets(linebuff, 512, file);
		}
	}
	//
	vector<NNUInt> indices;
	vector<Vertex> vertices;
	unordered_map<NNUInt, NNUInt> vertex_map;
	//
	unordered_map<NNUInt, FaceEdge> edge_map;
	const NNUInt face_count = NNUInt(positions_indices.size() / 3);
	//
	for (NNUInt f = 0; f < face_count; ++f)
	{
		NNVec3 ab = positions[positions_indices[f * 3 + 1]] - positions[positions_indices[f * 3 + 0]];
		NNVec3 bc = positions[positions_indices[f * 3 + 2]] - positions[positions_indices[f * 3 + 1]];
		NNVec3 normal = glm::normalize(glm::cross(ab, bc));
		for (int v = 0; v < 3; ++v)
		{
			//
			NNUInt ip = positions_indices[f * 3 + v];
			NNUInt it = texcoords_indices[f * 3 + v];
			NNUInt vmkey = (ip << 16) | it;
			if (vertex_map.find(vmkey) == vertex_map.end())
			{
				NNUInt idx = NNUInt(vertices.size());
				vertices.push_back(Vertex{ positions[ip] * 30.0f, normal, texcoords[it] });
				vertex_map[vmkey] = idx;
			}
			indices.push_back(vertex_map[vmkey]);
			//
			NNUInt s_i = positions_indices[f * 3 + ((v + 0) % 3)];
			NNUInt e_i = positions_indices[f * 3 + ((v + 1) % 3)];
			NNUInt emkey = (s_i << 16) | e_i;
			assert(edge_map.find(emkey) == edge_map.end());
			//
			edge_map[emkey] = FaceEdge { s_i, e_i, f, AdjacentEdge(v) } ;
		}
	}
	//
	m_source_face_adjacencies.clear();
	m_source_face_adjacencies.resize(face_count);
	for (const auto& keyvalue : edge_map)
	{
		const FaceEdge& cur_edge = keyvalue.second;
		const NNUInt adj_edge_key = (cur_edge.e_i << 16) | cur_edge.s_i;
		const auto it = edge_map.find(adj_edge_key);
		if (it != edge_map.end())
		{
			const FaceEdge& adj_edge = it->second;
			m_source_face_adjacencies[cur_edge.face][adj_edge.face] = FaceAdjacency { cur_edge.face, adj_edge.face, cur_edge.edge, adj_edge.edge };
		}
	}
	//
	m_source_mesh = Mesh::Create(vertices, indices, {});
}

NNUInt LappedTextureMesh::AddPatch()
{
	//
	LappedTexturePatch patch(m_source_mesh->GetIndexData(), m_source_mesh->GetVertexData(), m_source_face_adjacencies, m_candidate_faces);
	//
	m_patches.emplace_back(patch);
	//
	return NNUInt(m_patches.size() - 1);
}

void LappedTextureMesh::Draw()
{
	{
		m_patch_rendering_shader->Use();
		m_patch_texture->Use();
		for (auto it = m_patches.rbegin(); it != m_patches.rend(); ++it)
		{
			it->Draw();
		}
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

bool LappedTextureMesh::ReadSourceFaceAdjacencies()
{
	FILE *file = fopen(m_source_face_adjacencies_cachepath.c_str(), "r");
	if (file == nullptr)
	{
		return false;
	}
	//
	m_source_face_adjacencies.clear();
	//
	NNUInt src_face;
	while (fscanf(file, "%d", &src_face) != EOF)
	{
		m_source_face_adjacencies.push_back({});
		NNUInt num;
		fscanf(file, "%d", &num);
		for (NNUInt i = 0; i < num; ++i)
		{
			NNUInt src_edge, dst_face, dst_edge;
			fscanf(file, "%d %d %d", &src_edge, &dst_face, &dst_edge);
			m_source_face_adjacencies[src_face][dst_face] = FaceAdjacency{ src_face , src_edge, AdjacentEdge(dst_face), AdjacentEdge(dst_edge) };
		}
	}
	//
	fclose(file);
	//
	return true;
}

void LappedTextureMesh::WriteSourceFaceAdjacencies()
{
	FILE *file = fopen(m_source_face_adjacencies_cachepath.c_str(), "w");
	for (size_t i = 0; i < m_source_face_adjacencies.size(); ++i)
	{
		fprintf(file, "%d\n", int(i));
		const auto& adjacencies = m_source_face_adjacencies[i];
		fprintf(file, "%d   ", int(adjacencies.size()));
		for (const auto keyvalue : adjacencies)
		{
			fprintf(file, "%d %d %d  ", keyvalue.second.src_edge, keyvalue.second.dst_face, keyvalue.second.dst_edge);
		}
		fprintf(file, "\n");
	}
	fclose(file);
}

void LappedTextureMesh::BuildSourceFaceAdjacencies()
{
	//
	const auto& indices = m_source_mesh->GetIndexData();
	const auto& vertices = m_source_mesh->GetVertexData();
	//
	NNUInt face_num = NNUInt(indices.size() / 3);
	//
	m_source_face_adjacencies.clear();
	m_source_face_adjacencies.resize(face_num);
	//
	for (NNUInt src_face = 0; src_face < face_num; ++src_face)
	{
		dLog("[Coverage] Building %d adjacent face relation...", src_face);
		for (NNUInt dst_face = src_face + 1; dst_face < face_num; ++dst_face)
		{
			optional<FaceAdjacency> adj = CalcAdjacentEdge(indices, vertices, src_face, dst_face);
			if (adj.has_value())
			{
				m_source_face_adjacencies[src_face][dst_face] = FaceAdjacency{ src_face, dst_face, adj->src_edge, adj->dst_edge };
				m_source_face_adjacencies[dst_face][src_face] = FaceAdjacency{ dst_face, src_face, adj->dst_edge, adj->src_edge };
			}
			if (m_source_face_adjacencies[src_face].size() >= 3)
			{
				break;
			}
		}
	}
	//
}
