/*Copyright reserved by KenLee@2020 ken4000kl@gmail.com*/
#ifndef LAPPED_TEXTURE_PATCH
#define LAPPED_TEXTURE_PATCH

#include <set>
#include <deque>
#include <vector>
#include "NeneEngine/Nene.h"


class LappedTexturePatch
{
public:
	//
	LappedTexturePatch();
	~LappedTexturePatch();
	//
	void Grow(std::set<NNUInt>& candidate_faces);
	void DrawMesh(const NNDrawMode mode);
	void DrawEdge(const NNDrawMode mode);
	void Initialize(std::shared_ptr<Mesh> source_mesh, std::set<NNUInt>& candidate_faces);

private:
	//
	void AddFace(NNUInt face);
	//
	void UpdateForRendering();

private:
	// The patch vertex indices
	std::vector<NNUInt> m_indices;
	// The source mesh which the patch from
	std::shared_ptr<Mesh> m_source_mesh;
	// An edge is represented by 2 indices of vertices [i] and [i + 1]
	std::deque<NNUInt> m_outer_edges;

	// 
	std::shared_ptr<Mesh> m_patch_mesh;
	std::shared_ptr<Mesh> m_outer_edge_mesh;
};


#endif // LAPPED_TEXTURE_PATCH
