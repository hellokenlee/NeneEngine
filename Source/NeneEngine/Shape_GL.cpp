/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifdef NENE_GL

#include "Shape.h"
#include "Debug.h"

using namespace std;

Shape::Shape() : Drawable() {
	//
	mVAO = 0;
	mVBO = 0;
	mEBO = 0;
	//
	mVertexNum = 0;
	mIndexNum = 0;
	// 默认顶点格式
	mVertexFormat = POSITION;
	mDrawMode = NN_TRIANGLE;
}

Shape::~Shape() {
	if (mEBO != 0) glDeleteBuffers(1, &mEBO);
	if (mVBO != 0) glDeleteBuffers(1, &mVBO);
	if (mVAO != 0) glDeleteVertexArrays(1, &mVAO);
}

shared_ptr<Shape> Shape::Create(NNFloat* pVertices, NNUInt vArrayLen, NNVertexFormat vf) {
	//
	assert(vArrayLen > 0);
	// 检查长度对齐
	dLogIf(vArrayLen % vf != 0, "[Error]: Vertices Array's length(%d) is not aligned in decleared format(%d).\n",
		vArrayLen, vf);
	// 
	Shape* res = new Shape();
	res->mVertexNum = vArrayLen / vf;
	res->mVertexFormat = vf;
	// 创建顶点数组
	glGenVertexArrays(1, &(res->mVAO));
	// 申请显存
	glGenBuffers(1, &(res->mVBO));
	glBindVertexArray(res->mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, res->mVBO);
		// 写入顶点数据
		glBufferData(GL_ARRAY_BUFFER, vArrayLen * sizeof(GLfloat), pVertices, GL_STATIC_DRAW);
		// 根据顶点格式写入 Layout
		switch (vf) {
			case POSITION: {
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
				glEnableVertexAttribArray(0);
				break;
			}
			case POSITION_TEXTURE: {
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
				glEnableVertexAttribArray(1);
				break;
			}
			case POSITION_NORMAL: {
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
				glEnableVertexAttribArray(1);
				break;
			}
			case POSITION_NORMAL_TEXTURE: {
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
				glEnableVertexAttribArray(2);
				break;
			}
			default: {
				dLog("[Info]: Unknown Vertex Format(%d)\n", vf);
			}
		}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	//
	return shared_ptr<Shape>(res);
}

shared_ptr<Shape> Shape::Create(std::vector<NNFloat> vertices, NNVertexFormat vf) {
	return Create(vertices.data(), (NNUInt)vertices.size(), vf);
}

shared_ptr<Shape> Shape::Create(NNFloat* pVertices, NNUInt vArrayLen, NNUInt* pIndices, NNUInt iArrayLen, NNVertexFormat vf) {
	//
	assert(iArrayLen > 0);
	//
	shared_ptr<Shape> res = Create(pVertices, vArrayLen, vf);
	res->mIndexNum = iArrayLen;
	// 申请下标显存
	glGenBuffers(1, &(res->mEBO));
	// 绑定到顶点数组中
	glBindVertexArray(res->mVAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, res->mEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, iArrayLen * sizeof(GLuint), pIndices, GL_STATIC_DRAW);
	glBindVertexArray(0);
	//
	return res;
}

shared_ptr<Shape> Shape::Create(std::vector<NNFloat> vertices, std::vector<NNUInt> indices, NNVertexFormat vf) {
	return Create(vertices.data(), (NNUInt)vertices.size(), indices.data(), (NNUInt)indices.size(), vf);
}

void Shape::Draw(const shared_ptr<Shader> pShader, const shared_ptr<Camera> pCamera) {
	if (pShader != nullptr) {
		pShader->Use();
	}
	if (pCamera != nullptr) {
		pCamera->Use();
	}
	//
	NeneCB& CB = NeneCB::Instance();
	CB.PerObject().Data().model = mModelMat;
	CB.PerObject().Update(PER_OBJECT_SLOT);
	//
	glBindVertexArray(mVAO);
	if (mEBO != 0) {
		glDrawElements(mDrawMode, mIndexNum, GL_UNSIGNED_INT, 0);
	} else {
		glDrawArrays(mDrawMode, 0, mVertexNum);
	}
	glBindVertexArray(0);
}

void Shape::DrawInstanced(const shared_ptr<Shader> pShader, const shared_ptr<Camera> pCamera) {

}

void Shape::SetDrawMode(NNDrawMode newMode) {
	mDrawMode = newMode;
}

#endif