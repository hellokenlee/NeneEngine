/*Copyright reserved by KenLee@2020 ken4000kl@gmail.com*/
#ifndef LAPPED_TEXTURE_MESH
#define LAPPED_TEXTURE_MESH

#include <vector>
#include <unordered_set>
#include "LappedTexturePatch.h"

class LappedTextureMesh
{
public:
	//
	~LappedTextureMesh();
	LappedTextureMesh(const char* filepath);
	LappedTextureMesh(std::shared_ptr<Mesh> static_mesh);
	//
	bool IsFilled();
	NNUInt AddPatch();
	NNUInt PatchCount() { return NNUInt(m_patches.size()); }
	LappedTexturePatch& GetPatch(const NNUInt& i) { return m_patches[i]; }
	//
	void SetNeedToUpdateFaceCoverage() { m_need_to_update_coverage = true; };
	//
	void Draw();
	void DrawDebug(const NNUInt& i);
	void DrawAndSaveLappedCoord();
	void DrawAndCalcFaceCoverage();

private:
	void CreateShaderAndTextures();

	bool ReadSourceFaceAdjacencies();
	void WriteSourceFaceAdjacencies();
	void BuildSourceFaceAdjacencies();

	void ReadOBJFileAndBuildSourceFaceAdjacencies(const char* filepath);
	
private:
	//
	NNUInt m_source_face_count;
	std::shared_ptr<Mesh> m_source_mesh;
	//
	std::unordered_set<NNUInt> m_candidate_faces;
	std::vector<LappedTexturePatch> m_patches;
	//
	std::shared_ptr<Shape> m_debug_quad;
	std::shared_ptr<Texture2D> m_patch_texture;	
	std::shared_ptr<Shader> m_patch_debug_shader;
	std::shared_ptr<Shader> m_texture_debug_shader;
	std::shared_ptr<Shader> m_patch_rendering_shader;
	//
	bool m_need_to_update_coverage;
	std::shared_ptr<Mesh> m_coverage_mesh;
	std::shared_ptr<Shader> m_coverage_shader;
	std::shared_ptr<Shader> m_lapped_coord_shader;
	std::shared_ptr<RenderTarget> m_coverage_rtt;
	std::shared_ptr<RenderTarget> m_lapped_coord_rtt;
	//
	std::string m_source_face_adjacencies_cachepath;
	std::vector<std::unordered_map<NNUInt, FaceAdjacency>> m_source_face_adjacencies;
};


#endif // LAPPED_TEXTURE_MESH
