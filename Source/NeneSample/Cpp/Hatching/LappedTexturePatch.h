/*Copyright reserved by KenLee@2020 ken4000kl@gmail.com*/
#ifndef LAPPED_TEXTURE_PATCH
#define LAPPED_TEXTURE_PATCH

#include <set>
#include <map>
#include <deque>
#include <vector>
#include <optional>
#include "NeneEngine/Nene.h"
#include "LappedTextureUtility.h"


class LappedTexturePatch
{
public:
	//
	~LappedTexturePatch();
	LappedTexturePatch(const std::vector<NNUInt>& indices, const std::vector<Vertex>& vertices, const std::vector<std::map<NNUInt, FaceAdjacency>>& face_adjs, std::set<NNUInt>& faces);
	//
	void Grow();
	bool IsGrown() { return m_is_grown; }
	//
	void Draw() const;
	//
	void DrawCoverage() const;
	//
	void GenerateCoverageMesh();

private:
	
	//
	NNUInt AddSourceFaceToPatch(const NNUInt& sface);
	//
	NNUInt AddNearestSourceAdjacentFaceToPatch();
	//
	bool IsInsidePatchHull(const NNVec2& ta, const NNVec2& tb);
	//
	bool IsValidFaceAdjacency(const FaceAdjacency& adj);
		
private:
	//
	std::set<NNUInt>& m_candidate_faces;
	//
	const std::vector<NNUInt>& m_source_indices;
	const std::vector<Vertex>& m_source_vertices;
	const std::vector<std::map<NNUInt, FaceAdjacency>>& m_source_face_adjacencies;

private:
	//
	bool m_is_grown;
	bool m_is_optimazed;
	//
	NNVec3 m_center_position;
	std::vector<NNUInt> m_patch_indices;
	std::vector<Vertex> m_patch_vertices;
	//
	std::set<NNUInt> m_source_coverage_faces;
	std::map<NNUInt, NNUInt> m_source_to_patch_index;
	//
	std::shared_ptr<Mesh> m_patch_rendering_mesh;
	std::shared_ptr<Shape> m_patch_coverage_mesh;
};

#endif // LAPPED_TEXTURE_PATCH
