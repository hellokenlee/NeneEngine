/*Copyright reserved by KenLee@2020 ken4000kl@gmail.com*/
#ifndef LAPPED_TEXTURE_MESH
#define LAPPED_TEXTURE_MESH

#include <vector>
#include "LappedTexturePatch.h"

class LappedTextureMesh
{
public:
	//
	~LappedTextureMesh();
	LappedTextureMesh(std::shared_ptr<StaticMesh> static_mesh);
	//
	NNUInt AddPatch();
	NNUInt PatchCount() { return NNUInt(m_patches.size()); }
	LappedTexturePatch& GetPatch(const NNUInt& i) { return m_patches[i]; }
	//
	void Draw();
	void DrawDebug(const NNUInt& i);

private:
	//
	const StaticMesh& m_source;
	//
	std::set<NNUInt> m_candidate_faces;
	std::vector<LappedTexturePatch> m_patches;
	//
	std::shared_ptr<Shape> m_debug_quad;
	std::shared_ptr<Texture2D> m_patch_texture;	
	std::shared_ptr<Shader> m_patch_debug_shader;
	std::shared_ptr<Shader> m_texture_debug_shader;
	std::shared_ptr<Shader> m_patch_rendering_shader;
	
};


#endif // LAPPED_TEXTURE_MESH
