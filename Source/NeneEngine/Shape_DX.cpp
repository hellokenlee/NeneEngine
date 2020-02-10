/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifdef NENE_DX

#include "Shape.h"
#include "Debug.h"

using namespace std;

shared_ptr<Shape> Shape::Create(NNFloat* pVertices, NNUInt vArrayLen, NNVertexFormat vf) {
	// 检查
	assert(vArrayLen > 0);
	dLogIf(vArrayLen % vf != 0, "[Error]: Vertices Array's length(%d) is not aligned in decleared format(%d).\n",
		vArrayLen, vf);
	// 初始化
	Shape *res = new Shape();
	res->mVertexFormat = vf;
	res->mPerVertexSize = (UINT)vf * sizeof(FLOAT);
	res->mVertexNum = vArrayLen / vf;
	// 初始化缓冲描述符
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = res->mPerVertexSize * res->mVertexNum;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	// 初始化数据
	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
	initData.pSysMem = pVertices;
	// 创建顶点缓冲
	HRESULT hr = Utils::getDevice()->CreateBuffer(&vertexBufferDesc, &initData, &(res->mpVertexBuffer));
	// 检查
	if (FAILED(hr)) {
		dLog("[Error] Failed to Create Shape object.");
		delete res;
		return nullptr;
	}
	//
	return shared_ptr<Shape>(res);
}

shared_ptr<Shape> Shape::Create(NNFloat* pVertices, NNUInt vArrayLen, NNUInt* pIndices, NNUInt iArrayLen, NNVertexFormat vf) {
	// 创建顶点缓冲
	shared_ptr<Shape> res = Create(pVertices, vArrayLen, vf);
	// 初始化
	res->mIndexNum = iArrayLen;
	// 初始化索引缓冲描述符
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(UINT) * iArrayLen;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	// 初始化索引数据
	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
	initData.pSysMem = pIndices;
	// 写入到索引缓冲
	HRESULT hr = Utils::getDevice()->CreateBuffer(&indexBufferDesc, &initData, &(res->mpIndexBuffer));
	if (FAILED(hr)) {
		dLog("[Error] Failed to Create Shape object.");
		return nullptr;
	}
	return res;
}

shared_ptr<Shape> Shape::Create(vector<NNFloat> vertices, NNVertexFormat vf) {
	return Create(vertices.data(), vertices.size(), vf);
}

shared_ptr<Shape> Shape::Create(vector<NNFloat> vertices, vector<NNUInt> indices, NNVertexFormat vf) {
	return Create(vertices.data(), vertices.size(), indices.data(), indices.size(), vf);
}

Shape::Shape() {
	// 初始化
	mVertexNum = 0;
	mIndexNum = 0;
	// 默认格式
	mVertexFormat = POSITION;
	mDrawMode = NN_TRIANGLE;
	// 
	mpVertexBuffer = nullptr;
	mpIndexBuffer = nullptr;
	//
	mOffset = 0;
	mPerVertexSize = 0;
}

Shape::~Shape() {
	if (mpIndexBuffer) mpIndexBuffer->Release();
	if (mpVertexBuffer) mpVertexBuffer->Release();
}

void Shape::Draw(const shared_ptr<Shader> pShader, const shared_ptr<Camera> pCamera) {
	// 设置着色器和摄像机
	if (pShader != nullptr) {
		pShader->Use();
	}
	if (pCamera != nullptr) {
		pCamera->Use();
	}
	// 更新常量缓冲
	NeneCB& CB = NeneCB::Instance();
	CB.PerObject().data.model = mModelMat;
	CB.PerObject().Update(PER_OBJECT_SLOT);
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

void Shape::DrawInstanced(const shared_ptr<Shader> pShader, const shared_ptr<Camera> pCamera) {
	;
}

void Shape::SetDrawMode(NNDrawMode newMode) {
	mDrawMode = newMode;
}

#endif // NENE_DX