/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifdef NENE_DX

#include "Shader.h"
#include "IO.h"
#include "Debug.h"

#include <vector>
#include <d3dcompiler.h>

using namespace std;

#define DX_SHADER_COMPILE_FLAG (D3DCOMPILE_PACK_MATRIX_ROW_MAJOR | D3DCOMPILE_PACK_MATRIX_ROW_MAJOR) 

shared_ptr<Shader> Shader::Create(const NNChar *vsFilePath, const NNChar *fsFilePath, const NNVertexFormat lf, const bool& isLink) {
	//
	checkFileExist(vsFilePath);
	checkFileExist(fsFilePath);
	//
	Shader* res = new Shader();
	// 编译顶点着色器
	HRESULT hr1 = D3DCompileFromFile(IO::S2WS(string(vsFilePath)).c_str(),
		nullptr, nullptr, "VS", "vs_5_0", DX_SHADER_COMPILE_FLAG, 0, &(res->mpVSBlob), &(res->mpVSError));
	if (SUCCEEDED(hr1)) {
		Utils::getDevice()->CreateVertexShader(res->mpVSBlob->GetBufferPointer(), 
			res->mpVSBlob->GetBufferSize(), nullptr, &(res->mpVS));
		res->setLayoutFormat(lf);
	};
	dCall(res->checkCompileInfo(VERTEX_SHADER));
	// 编译像素着色器
	ID3DBlob *pPSBlob = nullptr;
	HRESULT hr2 = D3DCompileFromFile(IO::S2WS(string(fsFilePath)).c_str(),
		nullptr, nullptr, "PS", "ps_5_0", DX_SHADER_COMPILE_FLAG, 0, &pPSBlob, &(res->mpPSError));
	if (SUCCEEDED(hr2)) {
		Utils::getDevice()->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &(res->mpPS));
		pPSBlob->Release();
	}
	dCall(res->checkCompileInfo(FRAGMENT_SHADER));
	// 如果编译失败
	if (FAILED(hr1) || FAILED(hr2)) {
		delete res;
		return nullptr;
	}
	// 返回
	return shared_ptr<Shader>(res);
}

shared_ptr<Shader> Shader::createFromSource(const NNChar *vsSource, const NNChar *fsSource, const NNVertexFormat lf, const bool& isLink) {
	//
	Shader* res = new Shader();
	// 编译顶点着色器
	size_t vsSourceSize = strlen(vsSource);
	HRESULT hr1 = D3DCompile(vsSource, vsSourceSize, nullptr, nullptr, nullptr, 
		"VS", "vs_5_0", DX_SHADER_COMPILE_FLAG, 0, &(res->mpVSBlob), &(res->mpVSError));
	if (SUCCEEDED(hr1)) {
		Utils::getDevice()->CreateVertexShader(res->mpVSBlob->GetBufferPointer(),
			res->mpVSBlob->GetBufferSize(), nullptr, &(res->mpVS));
		res->setLayoutFormat(lf);
	};
	dCall(res->checkCompileInfo(VERTEX_SHADER));
	// 编译像素着色器
	ID3DBlob *pPSBlob = nullptr;
	size_t fsSourceSize = strlen(fsSource);
	HRESULT hr2 = D3DCompile(fsSource, fsSourceSize, nullptr, nullptr, nullptr,
		"PS", "ps_5_0", DX_SHADER_COMPILE_FLAG, 0, &pPSBlob, &(res->mpPSError));
	if (SUCCEEDED(hr2)) {
		Utils::getDevice()->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &(res->mpPS));
	}
	if (pPSBlob) {
		pPSBlob->Release();
	}
	dCall(res->checkCompileInfo(FRAGMENT_SHADER));
	// 如果编译失败
	if (FAILED(hr1) || FAILED(hr2)) {
		delete res;
		return nullptr;
	}
	// 返回
	return shared_ptr<Shader>(res);
}

Shader::Shader() {
	// 初始化
	mpVS = nullptr;
	mpHS = nullptr;
	mpCS = nullptr;
	mpGS = nullptr;
	mpPS = nullptr;
	mpVSError = nullptr;
	mpHSError = nullptr;
	mpCSError = nullptr;
	mpGSError = nullptr;
	mpPSError = nullptr;
	//
	mpVSBlob = nullptr;
	//
	mpLayout = nullptr;
	//
	mVF = UNDEFINE;
}

Shader::~Shader() {
	// 释放资源
	if (mpVS) mpVS->Release();
	if (mpHS) mpHS->Release();
	if (mpCS) mpCS->Release();
	if (mpGS) mpGS->Release();
	if (mpPS) mpPS->Release();
	//
	if (mpVSError) mpVSError->Release();
	if (mpHSError) mpHSError->Release();
	if (mpCSError) mpCSError->Release();
	if (mpGSError) mpGSError->Release();
	if (mpPSError) mpPSError->Release();
	//
	if (mpVSBlob) mpVSBlob->Release();
	//
	if (mpLayout)mpLayout->Release();
}

bool Shader::addOptionalShader(const NNChar *filePath, const NNShaderType st, const bool& isLink) {
	// 初始化
	ID3DBlob *pBlob = nullptr;
	HRESULT hrCompile, hrCreate;
	// 分类型处理
	switch (st) {
		case TESS_CONTROL_SHADER: {
			// 细分控制着色器：编译
			hrCompile = D3DCompileFromFile(IO::S2WS(string(filePath)).c_str(),
				nullptr, nullptr, "HS", "hs_5_0", DX_SHADER_COMPILE_FLAG, 0, &pBlob, &(mpHSError));
			// 创建
			if (SUCCEEDED(hrCompile)) {
				hrCreate = Utils::getDevice()->CreateHullShader(pBlob->GetBufferPointer(),
					pBlob->GetBufferSize(), nullptr, &mpHS);
			}
			break;
		}
		case TESS_EVALUATE_SHADER: {
			// 细分计算着色器：编译
			hrCompile = D3DCompileFromFile(IO::S2WS(string(filePath)).c_str(),
				nullptr, nullptr, "CS", "cs_5_0", DX_SHADER_COMPILE_FLAG, 0, &pBlob, &(mpCSError));
			// 创建
			if (SUCCEEDED(hrCompile)) {
				hrCreate = Utils::getDevice()->CreateComputeShader(pBlob->GetBufferPointer(),
					pBlob->GetBufferSize(), nullptr, &mpCS);
			}
			break;
		}
		case GEOMETRY_SHADER: {
			// 几何着色器：编译
			hrCompile = D3DCompileFromFile(IO::S2WS(string(filePath)).c_str(),
				nullptr, nullptr, "GS", "gs_5_0", DX_SHADER_COMPILE_FLAG, 0, &pBlob, &(mpGSError));
			// 创建
			if (SUCCEEDED(hrCompile)) {
				hrCreate = Utils::getDevice()->CreateGeometryShader(pBlob->GetBufferPointer(),
					pBlob->GetBufferSize(), nullptr, &mpGS);
			}
			break;
		}
		default: {
			dLog("[Error]: Unknown Shader Type(%d)\n", st);
			break;
		}
	}
	if (pBlob) {
		pBlob->Release();
	}
	dCall(checkCompileInfo(st));
	return (SUCCEEDED(hrCompile) && SUCCEEDED(hrCreate));
}

bool Shader::addOptionalShaderFromSource(const NNChar *source, const NNShaderType st, const bool& isLink) {
	return false;
}

void Shader::Use() {
	// 设置着色器
	Utils::getContext()->VSSetShader(mpVS, nullptr, 0);
	Utils::getContext()->HSSetShader(mpHS, nullptr, 0);
	Utils::getContext()->CSSetShader(mpCS, nullptr, 0);
	Utils::getContext()->GSSetShader(mpGS, nullptr, 0);
	Utils::getContext()->PSSetShader(mpPS, nullptr, 0);
	// 设置输入格式
	Utils::getContext()->IASetInputLayout(mpLayout);
}

bool Shader::checkCompileInfo(const NNShaderType st) {
	static string sName = "Unknow Shader";
	ID3DBlob *pShaderError = nullptr;
	switch (st) {
		case VERTEX_SHADER: {
			pShaderError = mpVSError;
			sName = "Vertex Shader";
			break;
		}
		case TESS_CONTROL_SHADER: {
			pShaderError = mpHSError;
			sName = "Tess Control Shader";
			break;
		}
		case TESS_EVALUATE_SHADER: {
			pShaderError = mpCSError;
			sName = "Tess Evaluate Shader";
			break;
		}
		case GEOMETRY_SHADER: {
			pShaderError = mpGSError;
			sName = "Geometry Shader";
			break;
		}
		case FRAGMENT_SHADER: {
			pShaderError = mpPSError;
			sName = "Fragment Shader";
			break;
		}
		default: {
			dLog("[Error]: Unknown Shader Type(%d)\n\n", st);
			return false;
		}
	}
	if (pShaderError) {
		dLog("[Error]: Can not comlile %s:\n%s\n\n", sName.c_str(), (const char*)pShaderError->GetBufferPointer());
		return false;
	}
	return true;
}

bool Shader::setLayoutFormat(NNVertexFormat vf) {
	// 清除之前的 Layout
	mVF = vf;
	if (mpLayout) mpLayout->Release();
	// 
	vector<D3D11_INPUT_ELEMENT_DESC> layoutDesc;
	//
	switch (vf) {
		case POSITION: {
			layoutDesc.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			break;
		}
		case POSITION_TEXTURE: {
			layoutDesc.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			layoutDesc.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			break;
		}
		case POSITION_NORMAL: {
			layoutDesc.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			layoutDesc.push_back({ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			break;
		}
		case POSITION_NORMAL_TEXTURE: {
			layoutDesc.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			layoutDesc.push_back({ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			layoutDesc.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
			break;
		}
		default: {
			dLog("[Error]: Unknown Layout Format!\n\n");
			break;
		}
	}
	//
	HRESULT hr = Utils::getDevice()->CreateInputLayout(layoutDesc.data(), layoutDesc.size(), 
		mpVSBlob->GetBufferPointer(), mpVSBlob->GetBufferSize(), &mpLayout);
	if (FAILED(hr)) {
		dLog("[Error] Vertex Shader's layout format does not match!\n\n");
		return false;
	}
	return true;
}

bool Shader::checkLinkInfo() {
	dLog("[Info] No need for linking in D3D mode.\n\n");
	return true;
}

#endif