/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef MESH_H
#define MESH_H

#include "Shader.h"
#include "Texture2D.h"
#include <vector>

//
//    网格顶点
//
struct MeshVertex {
	MeshVertex() {};
	NNVec3 mPosition;
	NNVec3 mNormal;
	NNVec2 mTexcoord;
};

//
//    网格类
//
class Mesh {
public:
	static std::shared_ptr<Mesh> Create(std::vector<MeshVertex>& vertices, std::vector<NNUInt>& indices,
		std::vector<std::shared_ptr<Texture2D>>& textures, std::vector<NNTextureType>& texTypes);
	~Mesh();
	void Draw();
	void drawInstance();
private:
	//
	Mesh();
	Mesh(const Mesh& rhs);
	Mesh& operator=(const Mesh& rhs);
	// 纹理指针
	std::vector<std::shared_ptr<Texture2D>> mTextures;
	// 纹理类型
	std::vector<NNTextureType> mTexureTypes;
	// 顶点数，索引数
	NNUInt mVertexNum, mIndexNum;
	// 顶点格式
	NNVertexFormat mVertexFormat;
	// 绘制模式
	NNDrawMode mDrawMode;
	// 顶点缓冲
#if defined NENE_GL
	NNUInt mVAO, mVBO, mEBO;
#elif defined NENE_DX
	ID3D11Buffer *mpVertexBuffer, *mpIndexBuffer;
	NNUInt mPerVertexSize, mOffset;
#else
	#error Please define NENE_GL or NENE_DX to select which Graphic-API you want to Use!
#endif
	friend class Model;
};

#endif // !MESH_H

