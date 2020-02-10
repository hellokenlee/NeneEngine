/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifdef NENE_GL

#include "Mesh.h"
using namespace std;

Mesh::Mesh() : mVAO(0), mVBO(0), mEBO(0),
	mDrawMode(NN_TRIANGLE), mVertexFormat(POSITION_NORMAL_TEXTURE), mVertexNum(0), mIndexNum(0)
{}

Mesh::~Mesh() {
	if (mVAO != 0) glDeleteVertexArrays(1, &mVAO);
	if (mVBO != 0) glDeleteBuffers(1, &mVBO);
	if (mIndexNum > 0 && mEBO != 0) glDeleteBuffers(1, &mEBO);
}

shared_ptr<Mesh> Mesh::Create(vector<MeshVertex>& vertices, vector<NNUInt>& indices,
	vector<shared_ptr<Texture2D>>& textures, vector<NNTextureType>& texTypes) {
	Mesh* ret = new Mesh();
	// 初始化
	ret->mTextures = textures;
	ret->mTexureTypes = texTypes;
	ret->mVAO = ret->mVBO = ret->mEBO = 0;
	ret->mIndexNum = (NNUInt)indices.size();
	ret->mVertexNum = (NNUInt)vertices.size();
	// 申请显存
	glGenVertexArrays(1, &(ret->mVAO));
	glGenBuffers(1, &(ret->mVBO));
	glGenBuffers(1, &(ret->mEBO));
	// 初始化显存中数据
	glBindVertexArray(ret->mVAO);
	// VBO
	glBindBuffer(GL_ARRAY_BUFFER, ret->mVBO);
	glBufferData(GL_ARRAY_BUFFER, ret->mVertexNum * sizeof(MeshVertex), &vertices[0], GL_STATIC_DRAW);
	// EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ret->mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ret->mIndexNum * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
	// 位置
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
	// 法向量
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// 纹理坐标
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	//
	glBindVertexArray(0);
	//
	return shared_ptr<Mesh>(ret);
}

void Mesh::Draw() {
	// 绑定纹理贴图
	for (NNUInt i = 0; i < mTextures.size(); ++i) {
		mTextures[i]->Use(mTexureTypes[i]);
	}
	// 设置顶点缓冲
	glBindVertexArray(mVAO);
	if (mEBO != 0) {
		glDrawElements(mDrawMode, mIndexNum, GL_UNSIGNED_INT, 0);
	}
	else {
		glDrawArrays(mDrawMode, 0, mVertexNum);
	}
	glBindVertexArray(0);
}

void Mesh::drawInstance() {

}

#endif