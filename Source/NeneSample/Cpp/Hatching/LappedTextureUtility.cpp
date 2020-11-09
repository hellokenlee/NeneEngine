/*Copyright reserved by KenLee@2020 ken4000kl@gmail.com*/

#include "LappedTextureUtility.h"

using namespace std;

#define PRECISION 1000000
#define INV_PRECISION 0.000001f


Eigen::Matrix2x3f CalcLinearTransformWithEigen(const Eigen::Vector3f& a, const Eigen::Vector3f& b, const Eigen::Vector3f& c, const Eigen::Vector2f& ta, const Eigen::Vector2f& tb, const Eigen::Vector2f& tc)
{
	//
	static const Eigen::Vector3f e0(1.f, 0.f, 0.0f);
	static const Eigen::Vector3f e1(0.f, 1.f, 0.0f);
	static const Eigen::Vector3f e2(0.f, 0.f, 1.0f);
	//
	Eigen::Matrix3f lsm;
	lsm << a, b, c;
	//
	Eigen::Vector3f xa = lsm.colPivHouseholderQr().solve(e0);
	Eigen::Vector2f col0 = xa[0] * ta + xa[1] * tb + xa[2] * tc;

	Eigen::Vector3f xb = lsm.colPivHouseholderQr().solve(e1);
	Eigen::Vector2f col1 = xb[0] * ta + xb[1] * tb + xb[2] * tc;

	Eigen::Vector3f xc = lsm.colPivHouseholderQr().solve(e2);
	Eigen::Vector2f col2 = xc[0] * ta + xc[1] * tb + xc[2] * tc;
	//
	Eigen::Matrix2x3f phi;
	phi << col0, col1, col2;
	//
	return phi;
}

/* Check whether P and Q lie on the same side of line AB */
float Side(NNVec2 p, NNVec2 q, NNVec2 a, NNVec2 b)
{
	float z1 = (b.x - a.x) * (p.y - a.y) - (p.x - a.x) * (b.y - a.y);
	float z2 = (b.x - a.x) * (q.y - a.y) - (q.x - a.x) * (b.y - a.y);
	return z1 * z2;
}

/* Check whether segment P0P1 intersects with triangle t0t1t2 */
IntersectStatus Intersect(NNVec2 p0, NNVec2 p1, NNVec2 t0, NNVec2 t1, NNVec2 t2)
{
	/* Check whether segment is outside one of the three half-planes
	 * delimited by the triangle. */
	float f1 = Side(p0, t2, t0, t1), f2 = Side(p1, t2, t0, t1);
	float f3 = Side(p0, t0, t1, t2), f4 = Side(p1, t0, t1, t2);
	float f5 = Side(p0, t1, t2, t0), f6 = Side(p1, t1, t2, t0);
	/* Check whether triangle is totally inside one of the two half-planes
	 * delimited by the segment. */
	float f7 = Side(t0, t1, p0, p1);
	float f8 = Side(t1, t2, p0, p1);

	/* If segment is strictly outside triangle, or triangle is strictly
	 * apart from the line, we're not intersecting */
	if ((f1 < 0 && f2 < 0) || (f3 < 0 && f4 < 0) || (f5 < 0 && f6 < 0)
		|| (f7 > 0 && f8 > 0))
		return IntersectStatus::OUTSIDE;

	/* If segment is aligned with one of the edges, we're overlapping */
	if ((f1 == 0 && f2 == 0) || (f3 == 0 && f4 == 0) || (f5 == 0 && f6 == 0))
		return IntersectStatus::OVERLAPPING;

	/* If segment is outside but not strictly, or triangle is apart but
	 * not strictly, we're touching */
	if ((f1 <= 0 && f2 <= 0) || (f3 <= 0 && f4 <= 0) || (f5 <= 0 && f6 <= 0)
		|| (f7 >= 0 && f8 >= 0))
		return IntersectStatus::TOUCHING;

	/* If both segment points are strictly inside the triangle, we
	 * are not intersecting either */
	if (f1 > 0 && f2 > 0 && f3 > 0 && f4 > 0 && f5 > 0 && f6 > 0)
		return IntersectStatus::INSIDE;

	/* Otherwise we're intersecting with at least one edge */
	return IntersectStatus::INTERSECTING;
}

NNVec3 Barycentric2Cartesian(const NNVec3& a, const NNVec3& b, const NNVec3& c, const NNVec3& barycentric)
{
	return NNVec3(a * barycentric.x + b * barycentric.y + c * barycentric.z);
}

void CalcTangentAndBitangent(const NNVec3& a, const NNVec3& b, const NNVec3& c, const NNVec3& normal, NNVec3& tangent, NNVec3& bitangent)
{
	static const NNVec3 up(0.0f, 1.0f, 0.0f);
	tangent = NNNormalize(up - (NNDot(up, normal) * normal));
	bitangent = NNNormalize(NNCross(tangent, normal));
}

bool IsNearlySame(const NNVec2& pos0, const NNVec2& pos1)
{
	auto result = glm::epsilonEqual(pos0, pos1, INV_PRECISION);
	return result.x && result.y;
}

bool IsNearlySame(const NNVec3& pos0, const NNVec3& pos1)
{
	auto result = glm::epsilonEqual(pos0, pos1, INV_PRECISION);
	return result.x && result.y && result.z;
}

NNVec3 RandomPositionInTriangle(const NNVec3& a, const NNVec3& b, const NNVec3& c)
{
	float b0 = float(rand() % PRECISION) / float(PRECISION);
	float b1 = float(rand() % PRECISION) / float(PRECISION);
	float b2 = float(rand() % PRECISION) / float(PRECISION);
	NNVec3 barycentric(b0, b1, b2);
	return Barycentric2Cartesian(a, b, c, barycentric);;
}

NNVec2 SimilarTriangle3DTo2D(NNVec3 a_3d, NNVec3 b_3d, NNVec3 c_3d, NNVec3 n_3d, NNVec2 a_2d, NNVec2 b_2d)
{
	//
	a_3d *= 1000.0f;
	b_3d *= 1000.0f;
	c_3d *= 1000.0f;
	//
	a_2d *= 1000.0f;
	b_2d *= 1000.0f;
	//
	NNFloat len_ab_3d = glm::distance(a_3d, b_3d);
	NNFloat len_ac_3d = glm::distance(a_3d, c_3d);
	NNFloat len_ab_2d = glm::distance(a_2d, b_2d);
	//
	NNVec3 dir_ab_3d = glm::normalize(b_3d - a_3d);
	NNVec3 dir_ac_3d = glm::normalize(c_3d - a_3d);
	//
	NNFloat angle_bac = glm::orientedAngle(dir_ab_3d, dir_ac_3d, -n_3d);
	//
	NNVec2 dir_ab_2d = glm::normalize(b_2d - a_2d);
	NNVec2 dir_ac_2d = glm::normalize(glm::rotate(dir_ab_2d, angle_bac));
	NNVec2 c_2d = a_2d + (dir_ac_2d * len_ac_3d * len_ab_2d / len_ab_3d);
	c_2d *= 0.001f;
	//
	c_2d = glm::clamp(c_2d, 0.0f, 1.0f);
	//
	return c_2d;
}
