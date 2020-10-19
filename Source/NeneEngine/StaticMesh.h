/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef MODEL_H
#define MODEL_H

#include <unordered_map>
#include <Assimp/scene.h>
#include <Assimp/postprocess.h>
#include <Assimp/Importer.hpp>

#include "Mesh.h"
#include "Drawable.h"

//
//    StaticMesh: 
//

class StaticMesh : public Drawable 
{
public:
	//
	static std::shared_ptr<StaticMesh> Create(const NNChar* filepath);
	//
	virtual void Draw(const std::shared_ptr<Shader> pShader = nullptr,
		const std::shared_ptr<Camera> pCamera = nullptr);
	virtual void DrawInstanced(const std::shared_ptr<Shader> pShader = nullptr,
		const std::shared_ptr<Camera> pCamera = nullptr);

	virtual std::vector<std::shared_ptr<Mesh>>& GetMeshes() { return m_meshes; };
protected:
	//
	virtual void ProcessNode(aiNode* pNode, const aiScene* pScene);
	virtual void ProcessMesh(aiMesh* pMesh, const aiScene* pScene);
	virtual void ProcessTexture(aiMaterial* pMaterial, aiTextureType aiType, NNTextureType nnType, std::vector<std::tuple<std::shared_ptr<Texture2D>, NNTextureType>>& textures);

protected:
	std::string m_dirpath;
	std::vector<std::shared_ptr<Mesh>> m_meshes;
	std::unordered_map<std::string, std::shared_ptr<Texture2D>> m_textures;

protected:
	StaticMesh() = default;
	StaticMesh(const StaticMesh& rhs) = delete;
	StaticMesh& operator=(const StaticMesh& rhs) = delete;
};

#endif // MODEL_H
