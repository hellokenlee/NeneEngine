/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/

#include "Model.h"
#include "IO.h"
#include "Debug.h"

using namespace std;

Model::Model() : Drawable() {
	mPath = "";
	mMeshes.clear();
	mCachedTexs.clear();
}

shared_ptr<Model> Model::Create(const NNChar* filepath) {
	//
	checkFileExist(filepath);
	// 载入器
	Assimp::Importer importer;
	// 获取场景指针 ( 执行三角化 )
	const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipWindingOrder);
	// 错误检测
	if (scene == nullptr || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr) {
		printf("[Error] Model Loading Error: %s\n\n", importer.GetErrorString());
		return nullptr;
	}
	// 记录文件目录以加载相对路径文件 ( 如纹理 )
	Model* ret = new Model();
	ret->mPath = string(filepath);
	size_t pos = ret->mPath.rfind('/');
	ret->mPath = ret->mPath.substr(0, pos + 1);
	// 
	dLog("[Info] ===== Loading model begined:  ===== \n");
	dLog("    Total %d meshes: \n", scene->mNumMeshes);
	// 从根节点开始遍历加载模型
	ret->processNode(scene->mRootNode, scene);
	//
	dLog("[Info] ===== Model loading finished. ===== \n\n");
	//!TODO: delete scene? 
	return shared_ptr<Model>(ret);
}

void Model::Draw(const shared_ptr<Shader> pShader, const shared_ptr<Camera> pCamera) {
	// 
	if (pShader) pShader->Use();
	if (pCamera) pCamera->Use();
	// 更新Model矩阵
	NeneCB::Instance().PerObject().Data().model = mModelMat;
	NeneCB::Instance().PerObject().Update(PER_OBJECT_SLOT);
	// 绘制所有网格
	for (NNUInt i = 0; i < mMeshes.size(); ++i) {
		mMeshes[i]->Draw();
	}
}

void Model::DrawInstanced(const shared_ptr<Shader> pShader, const shared_ptr<Camera> pCamera) {

}

void Model::processNode(aiNode* pNode, const aiScene* pScene) {
	// 
	dLog("    |-- Load %d meshes from a node. (%s)\n", pNode->mNumMeshes, pNode->mName.C_Str());
	// 处理当前节点网格
	for (NNUInt i = 0; i < pNode->mNumMeshes; ++i) {
		aiMesh* pMesh = pScene->mMeshes[pNode->mMeshes[i]];
		processMesh(pMesh, pScene);
	}
	// 递归处理子节点
	for (NNUInt i = 0; i < pNode->mNumChildren; ++i) {
		this->processNode(pNode->mChildren[i], pScene);
	}
}

void Model::processMesh(aiMesh* pMesh, const aiScene* pScene) {
	// 构造Mesh需要的数据
	vector<MeshVertex> vertices;
	vector<NNUInt> indices;
	vector<shared_ptr<Texture2D>> textures;
	vector<NNTextureType> texTypes;
	//
	dLog("        |-- Load %d vertices from a mesh.\n", pMesh->mNumVertices);
	// 处理顶点数据
	for (NNUInt i = 0; i < pMesh->mNumVertices; ++i) {
		MeshVertex vertex;
		NNVec3 tmpVec3;
		NNVec2 tmpVec2;
		// 位置
		tmpVec3.x = pMesh->mVertices[i].x;
		tmpVec3.y = pMesh->mVertices[i].y;
		tmpVec3.z = pMesh->mVertices[i].z;
		vertex.mPosition = tmpVec3;
		// 法线
		if (pMesh->mNormals) {
			tmpVec3.x = pMesh->mNormals[i].x;
			tmpVec3.y = pMesh->mNormals[i].y;
			tmpVec3.z = pMesh->mNormals[i].z;
			vertex.mNormal = tmpVec3;
		}
		// 纹理
		if (pMesh->mTextureCoords[0]) {
			tmpVec2.x = pMesh->mTextureCoords[0][i].x;
			tmpVec2.y = pMesh->mTextureCoords[0][i].y;
			vertex.mTexcoord = tmpVec2;
		}
		// 把顶点放进去
		vertices.push_back(vertex);
	}
	// 处理索引数据
	for (NNUInt i = 0; i < pMesh->mNumFaces; i++) {
		aiFace face = pMesh->mFaces[i];
		for (NNUInt j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}
	//处理纹理数据
	if (pMesh->mMaterialIndex >= 0) {
		aiMaterial* material = pScene->mMaterials[pMesh->mMaterialIndex];
		processTexs(material, aiTextureType_DIFFUSE, DIFFUSE, textures, texTypes);
		processTexs(material, aiTextureType_SPECULAR, SPECULAR, textures, texTypes);
		processTexs(material, aiTextureType_HEIGHT, NORMAL, textures, texTypes);
	}
	// Debug 输出
	dLog("        |-- Process mesh with:\n");
	dLog("            HasNormal   : %s.\n", pMesh->mNormals ? "Yes" : "No");
	dLog("            IndicesNum  : %zd\n", indices.size());
	dLog("            VerticesNum : %zd\n", vertices.size());
	dLog("            TexturesNum : %zd\n", textures.size());
	// 把生成的网格对象压入成员变量
	mMeshes.push_back(Mesh::Create(vertices, indices, textures, texTypes));
}

void Model::processTexs(aiMaterial* pMaterial, aiTextureType aiType, NNTextureType nnType, vector<shared_ptr<Texture2D>> &texs, vector<NNTextureType> &texTypes) {
	// 一次性读取同类型的纹理
	for (NNUInt i = 0; i < pMaterial->GetTextureCount(aiType); ++i) {
		// 获取纹理的文件名
		aiString texFileName;
		pMaterial->GetTexture(aiType, i, &texFileName);
		// 变成纹理的文件路径
		string texFilePath = mPath + texFileName.C_Str();
		// 检测之前是否读取过
		auto it = mCachedTexs.find(texFilePath);
		if (it == mCachedTexs.end()) {
			// 之前没读取过, 生成新的纹理对象
			shared_ptr<Texture2D> pNewTexture = Texture2D::Create(texFilePath.c_str());
			mCachedTexs.insert(make_pair(texFilePath, pNewTexture));
			texs.push_back(pNewTexture);
			dLog("            loaded new texture file (type: %d): %s\n", nnType, texFilePath.c_str());
		} else {
			// 之前已经读取过，避免重复读取
			texs.push_back(it->second);
			dLog("            loaded cached texture file (type: %d): %s\n", nnType, texFilePath.c_str());
		}
		// 写入该纹理的类型
		texTypes.push_back(nnType);
	}
}