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

enum AdjacencyCase
{
	AB = 0,
	BC = 1,
	CA = 2,
};

struct Adjacency
{
	NNUInt ori_patch_face;
	NNUInt new_patch_face;
	AdjacencyCase adj_case;
};

class LappedTexturePatch
{
public:
	//
	~LappedTexturePatch();
	LappedTexturePatch(const std::vector<NNUInt>& indices, const std::vector<Vertex> vertices, std::set<NNUInt>& faces);
	//
	void SetTexture(std::shared_ptr<Texture2D> tex) { m_patch_texture = tex; }
	//
	void Grow(std::set<NNUInt>& candidate_faces);
	bool IsGrown() { return m_is_grown; }
	//
	void Optimaze();
	//
	void DrawMesh();
	//
	void DrawAndUpdateFaceCoverage();
	//
	void Initialize(std::shared_ptr<Mesh> source_mesh, std::set<NNUInt>& candidate_faces, std::shared_ptr<Texture2D> patch_tex);
	

private:
	//
	bool IsValidAdjacency(const Adjacency& adjcency);
	//
	void AddSourceFaceToPatch(const NNUInt& face);
	void CheckAndAddCoveredFaceToPatch();
	std::optional<Adjacency> AddNearestAdjacentFaceToPatch();

private:
	//
	std::set<NNUInt>& m_candidate_faces;
	std::shared_ptr<Texture2D> m_patch_texture;
	const std::vector<NNUInt>& m_source_indices;
	const std::vector<Vertex>& m_source_vertices;

private:
	//
	bool m_is_grown;
	bool m_is_optimazed;
	//
	NNVec3 m_center_position;
	NNUInt m_patch_faces_num;
	std::vector<Vertex> m_patch_vertices;
	std::vector<NNUInt> m_patch_source_indices;
	//

private:
	//
	NNVec3 m_center;
	//


	// The patch vertex indices
	std::vector<NNUInt> m_indices;
	std::set<NNUInt> m_indices_set;
	std::map<NNUInt, Eigen::Matrix2x3f> m_face_phi;


	// The patch for rendering on texture
	std::shared_ptr<Mesh> m_patch_mesh;

	//
	std::shared_ptr<Texture2D> m_patch_tex;

	//
	std::vector<NNUInt> m_faces;
	std::shared_ptr<Shape> m_coverage_mesh;
	std::shared_ptr<RenderTarget> m_coverage_rtt;
	std::shared_ptr<Shader> m_shader_coverage;
	
};


#endif // LAPPED_TEXTURE_PATCH
