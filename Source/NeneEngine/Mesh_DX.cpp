/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifdef NENE_DX

#include "Debug.h"
#include "Mesh.h"

using namespace std;

Mesh::Mesh() : mpVertexBuffer(nullptr), mpIndexBuffer(nullptr),
	mPerVertexSize(0), mOffset(0), mVertexNum(0), mIndexNum(0),
	mDrawMode(NN_TRIANGLE), mVertexFormat(POSITION_NORMAL_TEXTURE)
{}

shared_ptr<Mesh> Mesh::Create(vector<MeshVertex>& vertices, vector<NNUInt>& indices,
	vector<std::shared_ptr<Texture2D>>& textures, vector<NNTextureType>& texTypes) {
	//
	ID3D11Buffer *pVertexBuffer, *pIndexBuffer;
	// 初始化索引缓冲描述符
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(UINT) * indices.size();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	// 初始化索引数据
	D3D11_SUBRESOURCE_DATA iInitData;
	ZeroMemory(&iInitData, sizeof(iInitData));
	iInitData.pSysMem = indices.data();
	// 写入到索引缓冲
	HRESULT hr = S_OK;
	hr = Utils::getDevice()->CreateBuffer(&indexBufferDesc, &iInitData, &pIndexBuffer);
	if (FAILED(hr)) {
		dLog("[Error] Failded to Create index buffer!");
		return nullptr;
	}
	// 初始化顶点缓冲描述符
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = POSITION_NORMAL_TEXTURE * sizeof(NNFloat) * vertices.size();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	// 初始化顶点数据
	D3D11_SUBRESOURCE_DATA vInitData;
	ZeroMemory(&vInitData, sizeof(vInitData));
	vInitData.pSysMem = vertices.data();
	// 写入到顶点缓冲
	hr = Utils::getDevice()->CreateBuffer(&vertexBufferDesc, &vInitData, &pVertexBuffer);
	if (FAILED(hr)) {
		dLog("[Error] Failded to Create vertex buffer.");
		pIndexBuffer->Release();
		return nullptr;
	}
	//
	Mesh* ret = new Mesh();
	ret->mDrawMode = NN_TRIANGLE;
	ret->mIndexNum = indices.size();
	ret->mVertexNum = vertices.size();
	ret->mTextures = textures;
	ret->mTexureTypes = texTypes;
	ret->mpIndexBuffer = pIndexBuffer;
	ret->mpVertexBuffer = pVertexBuffer;
	ret->mPerVertexSize = POSITION_NORMAL_TEXTURE * sizeof(NNFloat);
	//
	return shared_ptr<Mesh>(ret);
}

Mesh::~Mesh() {
	if (mpIndexBuffer)
		mpIndexBuffer->Release();
	if (mpVertexBuffer)
		mpVertexBuffer->Release();
}

void Mesh::Draw() {
	// 绑定纹理贴图
	for (NNUInt i = 0; i < mTextures.size(); ++i) {
		mTextures[i]->Use(mTexureTypes[i]);
	}
	// 设置顶点缓冲
	Utils::getContext()->IASetVertexBuffers(0, 1, &mpVertexBuffer, &mPerVertexSize, &mOffset);
	Utils::getContext()->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)mDrawMode);
	// 分模式绘制
	if (mpIndexBuffer != nullptr) {
		// 索引模式
		Utils::getContext()->IASetIndexBuffer(mpIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		Utils::getContext()->DrawIndexed(mIndexNum, 0, 0);
	} else {
		// 非索引模式
		Utils::getContext()->Draw(mVertexNum, 0);
	}
}

void Mesh::drawInstance() {

}

#endif