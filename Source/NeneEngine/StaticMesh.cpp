/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/

#include "IO.h"
#include "Debug.h"
#include "StaticMesh.h"

using namespace std;

string GetDirectoryPath(const NNChar* filepath)
{
	string result(filepath);
	size_t pos = result.rfind('/');
	return result.substr(0, pos + 1);
}

shared_ptr<StaticMesh> StaticMesh::Create(const NNChar* filepath, const NNFloat scale)
{
	//
	checkFileExist(filepath);
	// 载入器
	Assimp::Importer importer;
	// 读取文件
	const aiScene* scene = importer.ReadFile(filepath, aiProcess_GenUVCoords | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FixInfacingNormals);
	// 错误检测
	if (scene == nullptr || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr) {
		printf("[Error] Model Loading Error: %s\n\n", importer.GetErrorString());
		return nullptr;
	}
	// 文件目录
	StaticMesh* result = new StaticMesh();
	result->m_dirpath = GetDirectoryPath(filepath);
	// 
	dLog("[Info] ===== Loading model begined:  ===== \n");
	dLog("    Total %d meshes: \n", scene->mNumMeshes);
	// 从根节点开始遍历加载模型
	result->ProcessNode(scene->mRootNode, scene, scale);
	//
	dLog("[Info] ===== Model loading finished. ===== \n\n");
	//
	return shared_ptr<StaticMesh>(result);
}

void StaticMesh::Draw(const shared_ptr<Shader> shader, const shared_ptr<Camera> camera)
{
	// 
	if (shader) shader->Use();
	if (camera) camera->Use();
	// 更新矩阵
	NeneCB::Instance().PerObject().Data().model = mModelMat;
	NeneCB::Instance().PerObject().Update(PER_OBJECT_SLOT);
	// 绘制所有网格
	for (NNUInt i = 0; i < m_meshes.size(); ++i) 
	{
		m_meshes[i]->Draw();
	}
}

void StaticMesh::DrawInstanced(const shared_ptr<Shader> pShader, const shared_ptr<Camera> pCamera)
{

}

void StaticMesh::ProcessNode(aiNode* pNode, const aiScene* pScene, const NNFloat scale)
{
	// 
	dLog("    |-- Load %d meshes from a node. (%s)\n", pNode->mNumMeshes, pNode->mName.C_Str());
	// 处理当前节点网格
	for (NNUInt i = 0; i < pNode->mNumMeshes; ++i) 
	{
		aiMesh* pMesh = pScene->mMeshes[pNode->mMeshes[i]];
		ProcessMesh(pMesh, pScene, scale);
	}
	// 递归处理子节点
	for (NNUInt i = 0; i < pNode->mNumChildren; ++i)
	{
		this->ProcessNode(pNode->mChildren[i], pScene, scale);
	}
}

void StaticMesh::ProcessMesh(aiMesh* pMesh, const aiScene* pScene, const NNFloat scale) {
	// 构造Mesh需要的数据
	vector<Vertex> vertices;
	vector<NNUInt> indices;
	vector<tuple<shared_ptr<Texture2D>, NNTextureType>> textures;
	//
	dLog("        |-- Load %d vertices from a mesh.\n", pMesh->mNumVertices);
	// 处理顶点数据
	for (NNUInt i = 0; i < pMesh->mNumVertices; ++i) 
	{
		Vertex vertex;
		NNVec3 tmpVec3;
		NNVec2 tmpVec2;
		// 位置
		tmpVec3.x = pMesh->mVertices[i].x;
		tmpVec3.y = pMesh->mVertices[i].y;
		tmpVec3.z = pMesh->mVertices[i].z;
		vertex.m_position = tmpVec3 * scale;
		// 法线
		if (pMesh->mNormals)
		{
			tmpVec3.x = pMesh->mNormals[i].x;
			tmpVec3.y = pMesh->mNormals[i].y;
			tmpVec3.z = pMesh->mNormals[i].z;
			vertex.m_normal = tmpVec3;
		}
		// 纹理
		if (pMesh->mTextureCoords[0]) 
		{
			tmpVec2.x = pMesh->mTextureCoords[0][i].x;
			tmpVec2.y = pMesh->mTextureCoords[0][i].y;
			vertex.m_texcoord = tmpVec2;
		}
		// 把顶点放进去
		vertices.push_back(vertex);
	}
	// 处理索引数据
	for (NNUInt i = 0; i < pMesh->mNumFaces; i++) 
	{
		aiFace face = pMesh->mFaces[i];
		for (NNUInt j = 0; j < face.mNumIndices; j++) 
		{
			indices.push_back(face.mIndices[j]);
		}
	}
	// 处理纹理数据
	if (pMesh->mMaterialIndex >= 0) {
		aiMaterial* material = pScene->mMaterials[pMesh->mMaterialIndex];
		ProcessTexture(material, aiTextureType_DIFFUSE, DIFFUSE, textures);
		ProcessTexture(material, aiTextureType_SPECULAR, SPECULAR, textures);
		ProcessTexture(material, aiTextureType_HEIGHT, NORMAL, textures);
	}
	// Debug 输出
	dLog("        |-- Process mesh with:\n");
	dLog("            HasNormal   : %s.\n", pMesh->mNormals ? "Yes" : "No");
	dLog("            IndicesNum  : %zd\n", indices.size());
	dLog("            VerticesNum : %zd\n", vertices.size());
	dLog("            TexturesNum : %zd\n", textures.size());
	// 把生成的网格对象压入成员变量
	m_meshes.push_back(Mesh::Create(move(vertices), move(indices), move(textures)));
}

void StaticMesh::ProcessTexture(aiMaterial* pMaterial, aiTextureType aiType, NNTextureType nnType, vector<tuple<shared_ptr<Texture2D>, NNTextureType>>& textures)
{
	// 一次性读取同类型的纹理
	for (NNUInt i = 0; i < pMaterial->GetTextureCount(aiType); ++i)
	{
		// 获取纹理的文件名
		aiString texFileName;
		pMaterial->GetTexture(aiType, i, &texFileName);
		// 变成纹理的文件路径
		string texFilePath = m_dirpath + texFileName.C_Str();
		// 检测之前是否读取过
		auto it = m_textures.find(texFilePath);
		if (it == m_textures.end())
		{
			// 之前没读取过, 生成新的纹理对象
			shared_ptr<Texture2D> new_texture = Texture2D::Create(texFilePath.c_str());
			m_textures.insert(make_pair(texFilePath, new_texture));
			textures.push_back(tuple<shared_ptr<Texture2D>, NNTextureType>(new_texture, nnType));
			dLog("            loaded new texture file (type: %d): %s\n", nnType, texFilePath.c_str());
		} 
		else 
		{
			// 之前已经读取过，避免重复读取
			textures.push_back(tuple<shared_ptr<Texture2D>, NNTextureType>(it->second, nnType));
			dLog("            loaded cached texture file (type: %d): %s\n", nnType, texFilePath.c_str());
		}
	}
}