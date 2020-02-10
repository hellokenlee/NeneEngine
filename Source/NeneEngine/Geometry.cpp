/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/

#include "Geometry.h"
#include "Debug.h"
#include <unordered_map>

using namespace std;

struct NNUVec3 {
	NNUVec3(NNUInt _x, NNUInt _y, NNUInt _z) : x(_x), y(_y), z(_z) {}
	NNUInt x, y, z;
};

void InvertIndexOrder(NNUInt *indices, NNUInt iNum) {
	dLogIf(iNum % 3 != 0, "[Error] Inverting vertex order of a non-trianglized mesh may cause undefine behavior.\n");
	for (NNUInt v = 0; v < iNum / 3; ++v) {
		swap(indices[3 * v + 1], indices[3 * v + 2]);
	}
}

NNUInt CreateSphereIco_GetMiddle(unordered_map<NNULong, NNUInt> &vertexHash, vector<NNVec3> &vertices, NNUInt index1, NNUInt index2) {
	if (index1 > index2) {
		// 保证index1 < index2
		swap(index1, index2);
	}
	NNULong key = ((NNULong)index1 << 32) + index2;
	if (vertexHash.find(key) != vertexHash.end()) {
		// 如果已经计算过了该个点
		return vertexHash[key];
	}
	// 如果没有计算过这个点，新生成一个点
	NNVec3 newVertex(
		(vertices[index1].x + vertices[index2].x) / 2,
		(vertices[index1].y + vertices[index2].y) / 2,
		(vertices[index1].z + vertices[index2].z) / 2
	);
	//
	NNFloat len = sqrtf((newVertex.x * newVertex.x) + (newVertex.y * newVertex.y) + (newVertex.z * newVertex.z));
	// 单位化处理
	newVertex.x /= len;
	newVertex.y /= len;
	newVertex.z /= len;
	// 把新的顶点放入顶点集
	int res = int(vertices.size());
	vertices.push_back(newVertex);
	vertexHash[key] = res;
	//
	return res;
};

shared_ptr<Shape> Geometry::CreateSphereIco(const NNUInt& level, NNVertexOrder vo) {
	//
	assert(sizeof(NNUInt) == 4 && sizeof(NNULong) == 8);
	// 顶点数组
	vector<NNVec3> vertices;
	NNFloat t = (1.0f + sqrtf(5.0f)) / 2.0f;
	NNFloat len = sqrt(1.0f + (t * t));
	t /= len;
	NNFloat s = 1.0f / len;
	// 3个互相垂直的长方形
	// XoY平面上
	vertices.push_back(NNVec3(-s,  t, 0.0f));
	vertices.push_back(NNVec3( s,  t, 0.0f));
	vertices.push_back(NNVec3(-s, -t, 0.0f));
	vertices.push_back(NNVec3( s, -t, 0.0f));
	// YoZ平面上
	vertices.push_back(NNVec3(0.0f, -s,  t));
	vertices.push_back(NNVec3(0.0f,  s,  t));
	vertices.push_back(NNVec3(0.0f, -s, -t));
	vertices.push_back(NNVec3(0.0f,  s, -t));
	// XoZ平面上
	vertices.push_back(NNVec3( t, 0.0f, -s));
	vertices.push_back(NNVec3( t, 0.0f,  s));
	vertices.push_back(NNVec3(-t, 0.0f, -s));
	vertices.push_back(NNVec3(-t, 0.0f,  s));

	// 索引数组
	vector<NNUVec3> faces;
	// 根据上面的12个顶点构造20面体 ( 逆时针顺序 )
	// 0顶点相邻的5个三角面
	faces.push_back(NNUVec3(0, 5, 11));
	faces.push_back(NNUVec3(0, 1, 5));
	faces.push_back(NNUVec3(0, 7, 1));
	faces.push_back(NNUVec3(0, 10, 7));
	faces.push_back(NNUVec3(0, 11, 10));
	//
	faces.push_back(NNUVec3(1, 9, 5));
	faces.push_back(NNUVec3(5, 4, 11));
	faces.push_back(NNUVec3(11, 2, 10));
	faces.push_back(NNUVec3(10, 6, 7));
	faces.push_back(NNUVec3(7, 8, 1));
	// 3顶点附近的5个三角面
	faces.push_back(NNUVec3(3, 4, 9));
	faces.push_back(NNUVec3(3, 2, 4));
	faces.push_back(NNUVec3(3, 6, 2));
	faces.push_back(NNUVec3(3, 8, 6));
	faces.push_back(NNUVec3(3, 9, 8));
	//
	faces.push_back(NNUVec3(4, 5, 9));
	faces.push_back(NNUVec3(2, 11, 4));
	faces.push_back(NNUVec3(6, 10, 2));
	faces.push_back(NNUVec3(8, 7, 6));
	faces.push_back(NNUVec3(9, 1, 8));

	// 细分
	unordered_map<NNULong, NNUInt> verticesHash;
	for (NNUInt i = 0; i < level; ++i) {
		vector<NNUVec3> faces2;
		for (NNUInt f = 0; f < faces.size(); ++f) {
			// 原三角形的三个顶点编号
			NNUInt p1 = faces[f].x;
			NNUInt p2 = faces[f].y;
			NNUInt p3 = faces[f].z;
			NNUInt a = CreateSphereIco_GetMiddle(verticesHash, vertices, p1, p2);
			NNUInt b = CreateSphereIco_GetMiddle(verticesHash, vertices, p2, p3);
			NNUInt c = CreateSphereIco_GetMiddle(verticesHash, vertices, p3, p1);
			// abc为新的内接三角形： 一个大三角形P1P2P3 -> 四个小三角形(保持逆时针)
			faces2.push_back(NNUVec3(p1, a, c));
			faces2.push_back(NNUVec3(p2, b, a));
			faces2.push_back(NNUVec3(p3, c, b));
			faces2.push_back(NNUVec3(a,  b, c));
		}
		faces = faces2;// 细分后的面取代原来的面
	}
	if (vo == CLOCK_WISE) {
		InvertIndexOrder(&faces[0].x, (NNUInt)faces.size() * 3);
	}
	dLog("[Info] Create icosphere with level %d :\n", level);
	dLog("    Vertices Num: %zd \n", vertices.size());
	dLog("    Faces Num: %zd \n", faces.size());
	// 计算纹理坐标
	vector<NNFloat> verticesPNT;
	for (NNUInt f = 0; f < faces.size(); ++f) {
		NNFloat x[3], y[3], z[3];
		NNFloat u[3], v[3];
		bool isPole = false;
		int poleVertex = -1;
		for (NNUInt i = 0; i < 3; ++i) {
			int index = *(&(faces[f].x) + i);
			x[i] = vertices[index].x;
			y[i] = vertices[index].y;
			z[i] = vertices[index].z;
			// 计算法纹理坐标
			u[i] = 0.5f + (atan2(-z[i], -x[i]) / M_PI / 2.0f);
			v[i] = 0.5f - (asin(-y[i]) / M_PI);
			// 检测是否极点
			if (x[i] == 0.0 && z[i] == 0.0 && (y[i] == 1.0 || y[i] == -1.0)) {
				poleVertex = i;
				isPole = true;
			}
		}
		//修复球体边上的锯齿状纹理错误
		NNVec3 tmp = NNVec3(u[2], v[2], 0.0f) - NNVec3(u[0], v[0], 0.0f);

		NNVec3 texNormal = NNCross(-tmp, NNVec3(u[1], v[1], 0.0f) - NNVec3(u[0], v[0], 0.0f));
		
		if (texNormal.z < 0) {
			ddLog("    Triangle (%.2f, %.2f, %.2f) - (%.2f, %.2f, %.2f) - (%.2f, %.2f, %.2f):\n ", x[0], y[0], z[0], x[1], y[1], z[1], x[2], y[2], z[2]);
			ddLog("    Have UV:(%.2f, %.2f) , (%.2f, %.2f) , (%.2f, %.2f): \n", u[0], v[0], u[1], v[1], u[2], v[2]);
			for (int i = 0; i < 3; ++i) {
				if (u[i] < 0.5)
					u[i] += 1.0;
			}
			ddLog("    Fixed UV:(%.2f, %.2f) , (%.2f, %.2f) , (%.2f, %.2f): \n\n", u[0], v[0], u[1], v[1], u[2], v[2]);
		}
		if (isPole) {
			u[poleVertex] = (u[(poleVertex + 1) % 3] + u[(poleVertex + 2) % 3]) / 2;
		}
		//
		for (int i = 0; i < 3; ++i) {
			// 位置
			verticesPNT.push_back(x[i]);
			verticesPNT.push_back(y[i]);
			verticesPNT.push_back(z[i]);
			// 法线
			verticesPNT.push_back(x[i]);
			verticesPNT.push_back(y[i]);
			verticesPNT.push_back(z[i]);
			// 纹理
			verticesPNT.push_back(u[i]);
			verticesPNT.push_back(v[i]);
		}
	}
	dLog("    Total Render vertices number(without indexing): %zd \n\n", verticesPNT.size() / 8);
	return Shape::Create(verticesPNT, POSITION_NORMAL_TEXTURE);
}

// 生成一个极轴为X轴的UV球体
shared_ptr<Shape> Geometry::CreateSphereUV(const NNUInt& latLines, const NNUInt& longLines, NNVertexOrder vo) {
	// 顶点数
	NNUInt vNum = ((latLines - 2) * longLines) + 2;
	// 面片数
	NNUInt fNum = ((latLines - 3) * (longLines) * 2) + (longLines * 2);
	// 
	NNFloat sphereYaw = 0.0f;
	NNFloat spherePitch = 0.0f;
	// 初始化顶点数据
	vector<NNVec3> vertices(vNum);
	NNVec4 currVertPos = NNVec4(0.0f, 0.0f, 1.0f, 1.0f);
	vertices[0] = NNVec3(currVertPos);
	// 逐纬线计算
	for (NNUInt i = 0; i < latLines - 2; ++i) {
		spherePitch = (i + 1) * (M_PI / (NNFloat)(latLines - 1));
		NNMat4 rotX = NNMat4Identity;
		rotX = NNCreateRotationX(rotX, spherePitch);
		// 逐经线计算
		for (NNUInt j = 0; j < longLines; ++j) {
			// 通过单位Z+向量绕X轴旋转然后绕Z轴旋转得到
			sphereYaw = j * (M_PI_TIMES_2 / (NNFloat)longLines);
			NNMat4 rotZ = NNMat4Identity; 
			rotZ = NNCreateRotationZ(rotZ, sphereYaw);
			currVertPos = NNMat4MulVec4(rotZ, NNMat4MulVec4(rotX, NNVec4(0.0f, 0.0f, 1.0f, 1.0f)));
			vertices[i * longLines + j + 1] = NNNormalize(NNVec3(currVertPos));
		}
	} 
	// 收尾
	vertices[vNum - 1] = NNVec3(0.0f, 0.0f, -1.0f);
	// 把四边形拆分成两个三角形
	vector<NNUInt> indices(fNum * 3);
	int k = 0;
	for (NNUInt l = 0; l < longLines - 1; ++l) {
		indices[k] = 0;
		indices[k + 1] = l + 2;
		indices[k + 2] = l + 1;
		k += 3;
	}
	indices[k] = 0;
	indices[k + 1] = 1;
	indices[k + 2] = longLines;
	k += 3;
	//
	for (NNUInt i = 0; i < latLines - 3; ++i) {
		for (NNUInt j = 0; j < longLines - 1; ++j) {
			indices[k] = i * longLines + j + 1;
			indices[k + 1] = i * longLines + j + 2;
			indices[k + 2] = (i + 1) * longLines + j + 1;
			//
			indices[k + 3] = (i + 1) * longLines + j + 1;
			indices[k + 4] = i * longLines + j + 2;
			indices[k + 5] = (i + 1) * longLines + j + 2;
			// 下一个四边形
			k += 6;
		}
		indices[k] = (i * longLines) + longLines;
		indices[k + 1] = (i * longLines) + 1;
		indices[k + 2] = ((i + 1) * longLines) + longLines;
		//
		indices[k + 3] = ((i + 1) * longLines) + longLines;
		indices[k + 4] = (i * longLines) + 1;
		indices[k + 5] = ((i + 1) * longLines) + 1;
		// 下一个四边形
		k += 6;
	}
	//
	for (NNUInt l = 0; l < longLines - 1; ++l) {
		indices[k] = vNum - 1;
		indices[k + 1] = (vNum - 1) - (l + 2);
		indices[k + 2] = (vNum - 1) - (l + 1);
		k += 3;
	}
	//
	indices[k] = vNum - 1;
	indices[k + 1] = vNum - 2;
	indices[k + 2] = (vNum - 1) - longLines;
	//
	vector<NNFloat> verticesPNT;
	for (size_t i = 0; i < vertices.size(); ++i) {
		// POSITION
		verticesPNT.push_back(vertices[i].x);
		verticesPNT.push_back(vertices[i].y);
		verticesPNT.push_back(vertices[i].z);
		// NORMAL
		verticesPNT.push_back(vertices[i].x);
		verticesPNT.push_back(vertices[i].y);
		verticesPNT.push_back(vertices[i].z);
		// TEXTURE
		verticesPNT.push_back(0.5f + atan2f(vertices[i].z, vertices[i].x) / M_PI_TIMES_2);
		verticesPNT.push_back(0.5f - asinf(vertices[i].y) / M_PI);
	}
	//
	dLog("[Info] Create uv-sphere with %d latlines, %d longlines:\n", latLines, longLines);
	dLog("    Vertices Num: %zd \n", vertices.size());
	dLog("    Faces Num: %zd \n", indices.size() / 3);
	dLog("    Total Render vertices number: %zd \n\n", indices.size());
	//
	if (vo == CLOCK_WISE) {
		InvertIndexOrder(indices.data(), (NNUInt)indices.size());
	}
	//
	return Shape::Create(verticesPNT, indices, POSITION_NORMAL_TEXTURE);
}


shared_ptr<Shape> Geometry::CreateCube(NNVertexOrder vo) {
	// 立方体顶点
	static vector<NNFloat> vertices = {
		// POSITION_XYZ         NORMAL_XYZ          TEXTURE_UV
		//前面
		-1.0f, -1.0f,  1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f, 
		-1.0f,  1.0f,  1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
		 1.0f,  1.0f,  1.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
		 1.0f, -1.0f,  1.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
		// 后面
		-1.0f, -1.0f, -1.0f,   0.0f, 0.0f,-1.0f,   1.0f, 1.0f,
		 1.0f, -1.0f, -1.0f,   0.0f, 0.0f,-1.0f,   0.0f, 1.0f,
		 1.0f,  1.0f, -1.0f,   0.0f, 0.0f,-1.0f,   0.0f, 0.0f,
		-1.0f,  1.0f, -1.0f,   0.0f, 0.0f,-1.0f,   1.0f, 0.0f,
		// 上面
		-1.0f,  1.0f, -1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
		-1.0f,  1.0f,  1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
		 1.0f,  1.0f,  1.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
		 1.0f,  1.0f, -1.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
		// 下面
		-1.0f, -1.0f, -1.0f,   0.0f,-1.0f, 0.0f,   0.0f, 1.0f,
		-1.0f, -1.0f,  1.0f,   0.0f,-1.0f, 0.0f,   0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f,   0.0f,-1.0f, 0.0f,   1.0f, 0.0f,
		 1.0f, -1.0f, -1.0f,   0.0f,-1.0f, 0.0f,   1.0f, 1.0f,
		// 左边
		-1.0f, -1.0f, -1.0f,  -1.0f, 0.0f, 0.0f,   0.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,  -1.0f, 0.0f, 0.0f,   0.0f,  0.0f,
		-1.0f,  1.0f,  1.0f,  -1.0f, 0.0f, 0.0f,   1.0f,  0.0f,
		-1.0f,  1.0f, -1.0f,  -1.0f, 0.0f, 0.0f,   1.0f, -1.0f,
		// 右边
		 1.0f, -1.0f, -1.0f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
		 1.0f,  1.0f,  1.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
		 1.0f,  1.0f, -1.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f
	};
	// 立方体索引
	static vector<NNUInt> indices = {
		// Front Face
		0,  1,  2,
		0,  2,  3,
		// Back Face
		4,  5,  6,
		4,  6,  7,
		// Top Face
		8, 10,  9,
		8, 11, 10,
		// Bottom Face
		12, 13, 14,
		12, 14, 15,
		// Left Face
		16, 18, 17,
		16, 19, 18,
		// Right Face
		20, 21, 22,
		20, 22, 23
	};
	if (vo == CLOCK_WISE) {
		InvertIndexOrder(indices.data(), (NNUInt)indices.size());
	}
	return Shape::Create(vertices, indices, POSITION_NORMAL_TEXTURE);
}

shared_ptr<Shape> Geometry::CreateQuad(NNVertexOrder vo) {
	vector<NNFloat> vertices = {
		// POSITION-XYZ        NORMAL-XYZ        TEXTURE-UV
		 1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,   0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f
	};
	vector<NNUInt> indices = {
		0, 2, 1,
		0, 3, 2
	};
	if (vo == CLOCK_WISE) {
		InvertIndexOrder(indices.data(), (NNUInt)indices.size());
	}
	return Shape::Create(vertices, indices, POSITION_NORMAL_TEXTURE);
}

void Geometry::CalcNormals(vector<NNFloat> &vertices, vector<NNUInt> &indices) {
	dLog("[Info] Generating Normals...\n");
	// 初始化
	vector<NNVec3> faceNormals;
	NNUInt facesNum = (NNUInt)indices.size() / 3;
	NNUInt verticesNum = (NNUInt)vertices.size() / POSITION_NORMAL_TEXTURE;
	NNFloat x, y, z;
	NNVec3 edge1, edge2, trivalNormal;
	// 遍历每个面， 求法向量
	for (NNUInt i = 0; i < facesNum; ++i) {
		// 获取表示一条边的向量 (edge 0,2)
		x = vertices[indices[(i * 3)] + 0] - vertices[indices[(i * 3) + 2] + 0];
		y = vertices[indices[(i * 3)] + 1] - vertices[indices[(i * 3) + 2] + 1];
		z = vertices[indices[(i * 3)] + 2] - vertices[indices[(i * 3) + 2] + 2];
		edge1 = NNVec3(x, y, z);
		// 获取表示一条边的向量 (edge 2,1)
		x = vertices[indices[(i * 3) + 2] + 0] - vertices[indices[(i * 3) + 1] + 0];
		y = vertices[indices[(i * 3) + 2] + 1] - vertices[indices[(i * 3) + 1] + 1];
		z = vertices[indices[(i * 3) + 2] + 2] - vertices[indices[(i * 3) + 1] + 2];
		edge2 = NNVec3(x, y, z);
		// 叉乘求面的法向量(!NOTE: 未单位化)
		//trivalNormal = edge1.Cross(edge2);
		// 保存该面的向量
		faceNormals.push_back(trivalNormal);
	}
	//
	NNVec3 normalSum;
	int facesUsing;
	// 一个点的法向量为包含这个点的面的法向量的平均
	for (NNUInt i = 0; i < verticesNum; ++i) {
		normalSum = NNVec3(0.0f, 0.0f, 0.0f);
		facesUsing = 0;
		// 检测那些面用到了这个点
		for (NNUInt j = 0; j < facesNum; ++j) {
			if (indices[j * 3] == i || indices[(j * 3) + 1] == i || indices[(j * 3) + 2] == i) {
				normalSum.x = faceNormals[j].x;
				normalSum.y = faceNormals[j].y;
				normalSum.z = faceNormals[j].z;
				facesUsing++;
			}
		}
		// 求平均
		normalSum = normalSum / (NNFloat)facesUsing;
		// 单位化
		//normalSum.Normalize();
		// 写入顶点法向量 - NORMALS_XYZ
		vertices[i * POSITION_NORMAL_TEXTURE + 3] = normalSum.x;
		vertices[i * POSITION_NORMAL_TEXTURE + 4] = normalSum.y;
		vertices[i * POSITION_NORMAL_TEXTURE + 5] = normalSum.z;
	}
	dLog("Done!\n");
}
