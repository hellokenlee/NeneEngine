/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef MODEL_H
#define MODEL_H

#include <unordered_map>
#include <Assimp/scene.h>
#include <Assimp/postprocess.h>
#include <Assimp/Importer.hpp>

#include "Mesh.h"
#include "Drawable.h"

class Model : public Drawable {
public:
	//
	static std::shared_ptr<Model> Create(const NNChar* filepath);
	// 绘制
	virtual void Draw(const std::shared_ptr<Shader> pShader = nullptr,
		const std::shared_ptr<Camera> pCamera = nullptr);
	virtual void DrawInstanced(const std::shared_ptr<Shader> pShader = nullptr,
		const std::shared_ptr<Camera> pCamera = nullptr);
	//
private:
	void processNode(aiNode* pNode, const aiScene* pScene);
	void processMesh(aiMesh* pMesh, const aiScene* pScene);
	void processTexs(aiMaterial* pMaterial, aiTextureType aiType, NNTextureType nnType,
		std::vector<std::shared_ptr<Texture2D>> &texs, std::vector<NNTextureType> &texTypes);
	//
	std::string mPath;
	std::vector<std::shared_ptr<Mesh>> mMeshes;
	std::unordered_map<std::string, std::shared_ptr<Texture2D>> mCachedTexs;
private:
	Model();
	Model(const Model& rhs);
	Model& operator=(const Model& rhs);
};

#endif // MODEL_H
