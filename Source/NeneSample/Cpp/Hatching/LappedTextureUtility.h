/*Copyright reserved by KenLee@2020 ken4000kl@gmail.com*/
#ifndef LAPPED_TEXTURE_UTILITY
#define LAPPED_TEXTURE_UTILITY

#include <set>
#include <optional>
#include "NeneEngine/Nene.h"
#include "NeneEngine/Eigen/Dense"


namespace Eigen
{
	typedef Matrix<float, 2, 3> Matrix2x3f;
}

enum AdjacentEdge
{
	AB = 0,
	BC = 1,
	CA = 2,
};

struct FaceAdjacency
{
	NNUInt src_face;
	NNUInt dst_face;
	AdjacentEdge src_edge;
	AdjacentEdge dst_edge;
};

#define SRC_ADJ_SHARE_I0(f, edge) (f * 3 + ((0 + edge) % 3))
#define SRC_ADJ_SHARE_I1(f, edge) (f * 3 + ((1 + edge) % 3))
#define SRC_ADJ_DIAGO_I2(f, edge) (f * 3 + ((2 + edge) % 3))

#define DST_ADJ_SHARE_I0(f, edge) (f * 3 + ((1 + edge) % 3))
#define DST_ADJ_SHARE_I1(f, edge) (f * 3 + ((0 + edge) % 3))
#define DST_ADJ_DIAGO_I2(f, edge) (f * 3 + ((2 + edge) % 3))

enum IntersectStatus
{
	INSIDE = 0,
	INTERSECTING,
	OVERLAPPING,
	TOUCHING,
	OUTSIDE,
};


bool IsNearlySame(const NNVec2& pos0, const NNVec2& pos1);

bool IsNearlySame(const NNVec3& pos0, const NNVec3& pos1);

NNVec3 RandomPositionInTriangle(const NNVec3& a, const NNVec3& b, const NNVec3& c);

NNVec2 SimilarTriangle3DTo2D(NNVec3 a_3d, NNVec3 b_3d, NNVec3 c_3d, NNVec3 n_3d, NNVec2 a_2d, NNVec2 b_2d);

void CalcTangentAndBitangent(const NNVec3& a, const NNVec3& b, const NNVec3& c, const NNVec3& normal, NNVec3& tangent, NNVec3& bitangent);

IntersectStatus Intersect(NNVec2 p0, NNVec2 p1, NNVec2 t0, NNVec2 t1, NNVec2 t2);

Eigen::Matrix2x3f CalcLinearTransformWithEigen(const Eigen::Vector3f& a, const Eigen::Vector3f& b, const Eigen::Vector3f& c, const Eigen::Vector2f& ta, const Eigen::Vector2f& tb, const Eigen::Vector2f& tc);

optional<FaceAdjacency> CalcAdjacentEdge(const std::vector<NNUInt> indices, const std::vector<Vertex> vertices, const NNUInt src_face, const NNUInt dst_face);

#endif // LAPPED_TEXTURE_UTILITY
