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
	const LappedTexturePatch& AddPatch();
	const LappedTexturePatch& GetPatch(const NNUInt& i) { return m_patches[i]; }
	//

private:
	//
	const StaticMesh& m_source;
	//
	std::set<NNUInt> m_candidate_faces;
	std::vector<LappedTexturePatch> m_patches;
};


#endif // LAPPED_TEXTURE_MESH
