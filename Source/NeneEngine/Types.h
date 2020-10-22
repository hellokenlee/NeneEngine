/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef TYPES_H
#define TYPES_H

//
//    Types: Data Types Used inside Engine
//

// Commons
#ifndef M_PI
#define M_PI 3.141592653f
#endif
#define M_PI_TIMES_2 6.283185306f
#define M_PI_OVER_2 1.5707963265f

//
enum NNLightType {
	POINT_LIGHT, DIRECTIONAL_LIGHT, SPOT_LIGHT, AREA_LIGHT, 
	NNLightTypeCount
};

// 顶点格式
enum NNVertexFormat {
	UNDEFINE = 0, POSITION = 3, POSITION_TEXTURE = 5, POSITION_NORMAL = 6, POSITION_NORMAL_TEXTURE = 8,
	NNVertexFormatNum = 5
};

// 纹理种类
enum NNTextureType {
	//
	DIFFUSE = 0, SPECULAR, REFLECT, NORMAL, ROUGHNESS,
	HEIGHT, AMBIENT_OCCLUSION,
	// 
	PBR_ALBEDO, PBR_METALNESS, PBR_ROUGHNESS, PBR_NORMAL,
	//
	NNTextureTypeNum,
};

// 纹理模式
enum NNTextureMode {
	AS_COLOR = 0, AS_DEPTH, AS_STENCIL,
	NNTextureModeNum
};

// 绘制模式
enum NNPolygonMode {
	FILL_CULL = 0, FILL_NO_CULL, WIRE_FRAME, VERTEX,
	NNPolygonModeNum
};

// 着色器类型
enum NNShaderType {
	VERTEX_SHADER = 0, FRAGMENT_SHADER, GEOMETRY_SHADER, 
	TESS_CONTROL_SHADER, TESS_EVALUATE_SHADER, 
	NNShaderTypeNum
};

// 键盘按键状态
enum NNKeyState {
	RELEASED = 0, PRESSED,
	NNKeyStateNum
};

// 按键动作
enum NNEventCode {
	UNDEFINED = 0, ON_KEY_PRESS, ON_KEY_RELEASE, ON_KEY_REPEAT, ON_MOUSE_MOVE, ON_MOUSE_SCROLL,
	NNEventCodeNum,
};

// 像素种类
enum NNColorFormat {
	RGBA = 0, BGRA, RGB, BGR,
	NNColorFormatNum,
};

enum NNVertexOrder {
	CLOCK_WISE = 0, COUNTER_CLOCK_WISE,
	NNVertexOrderNum
};

// 连接字符串
#define CONNECTION2(text1, text2) text1##text2
#define CONNECT2(text1, text2) CONNECTION2(text1, text2)

// Cross GAPI Platform
#if defined NENE_GL // OpenGL Version
	// GL Headers
	#define GLEW_STATIC
	#include <GL/glew.h>
	#include <GLFW/glfw3.h>
	#define GLM_FORCE_RADIANS
	#include "GLM/glm.hpp"
	#include "GLM/gtc/matrix_transform.hpp"
	#include "GLM/gtc/type_ptr.hpp"
	#include "GLM/gtc/epsilon.hpp"
	#include "GLM/gtx/rotate_vector.hpp"
	#include "GLM/gtx/vector_angle.hpp"

	// Types
	typedef GLchar NNChar;
	typedef GLubyte NNByte;
	typedef GLint NNInt;
	typedef GLuint NNUInt;
	typedef GLint64 NNLong;
	typedef GLfloat NNFloat;
	typedef GLuint64 NNULong;
	typedef glm::vec2 NNVec2;
	typedef glm::vec3 NNVec3;
	typedef glm::vec4 NNVec4;
	typedef glm::mat4 NNMat4;
	typedef glm::mat3 NNMat3;

	// Math Functions
	#define NNMat4Identity glm::mat4(1.0f)
	#define NNRadians(degree) glm::radians(degree)
	#define NNCreateTranslation(pos) glm::translate(NNMat4Identity, pos)
	#define NNCreateRotationX(mat, angle) glm::rotate(mat, angle, NNVec3(1.0f, 0.0f, 0.0f))
	#define NNCreateRotationY(mat, angle) glm::rotate(mat, angle, NNVec3(0.0f, 1.0f, 0.0f))
	#define NNCreateRotationZ(mat, angle) glm::rotate(mat, angle, NNVec3(0.0f, 0.0f, 1.0f))
	#define NNCreateScale(scaleVec) glm::scale(NNMat4Identity, scaleVec)
	#define NNCreateLookAt(position, target, up) glm::lookAt(position, target, up)
	#define NNCreatePerspective(fov, ratio, near, far) glm::perspective(fov, ratio, near, far)
	#define NNCreateOrtho(left, right, bottom, top, near, far) glm::ortho(left, right, bottom, top, near, far)
	#define NNCross(vec1, vec2) glm::cross(vec1, vec2)
	#define NNNormalize(vec1) glm::normalize(vec1)
	#define NNDot(vec1, vec2) glm::dot(vec1, vec2)
	#define NNMat4ValPtr(mat) mat
	#define NNMat4MulVec4(mat, vec) mat * vec
	#define NNMat4Inverse(mat) glm::inverse(mat)
	#define NNEpsilonEqual(vec1, vec2, epsilon) glm::epsilonEqual()
	#define NNEpsilon(gen_type) glm::epsilon<gen_type>()
	//
	enum NNMouseButton {
		NN_MOUSE_BUTTON_LEFT = GLFW_MOUSE_BUTTON_LEFT,
		NN_MOUSE_BUTTON_MID = GLFW_MOUSE_BUTTON_MIDDLE,
		NN_MOUSE_BUTTON_RIGHT = GLFW_MOUSE_BUTTON_RIGHT,
		NN_MOUSE_BUTTON_EXT = GLFW_MOUSE_BUTTON_LAST
	};
	// 图元种类
	enum NNDrawMode {
		NN_POINT = GL_POINTS,
		NN_LINE = GL_LINES,
		NN_LINE_STRIP = GL_LINE_STRIP,
		NN_LINE_LOOP = GL_LINE_LOOP,
		NN_TRIANGLE = GL_TRIANGLES,
		NN_TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
	};
	//
	enum NNTestFunc {
		NEVER = GL_NEVER,
		LESS = GL_LESS,
		EQUAL = GL_EQUAL,
		LEQUAL = GL_LEQUAL,
		GREATER = GL_GREATER,
		NOTEQUAL = GL_NOTEQUAL,
		GEQUAL = GL_GEQUAL,
		ALWAYS = GL_ALWAYS,
	};
	enum NNStencilOp {
		KEEP = GL_KEEP,
		ZERO = GL_ZERO,
		REPLACE = GL_REPLACE,
		INCREASE = GL_INCR,
		DECREASE = GL_DECR,
		INVERT = GL_INVERT,
		INCREASE_WRAP = GL_INCR_WRAP,
		DECREASE_WRAP = GL_DECR_WRAP
	};
	//
	#define NNWindow GLFWwindow*
	//
	#define NNKeyMap(key) CONNECT2(GLFW_KEY_, key)
#elif defined NENE_DX // D3D11 Version
	#define DIRECTINPUT_VERSION 0x0800	
	// DX Headers
	#include <d3d11.h>
	#include <DirectXMath.h>
	#include "DXM/SimpleMath.hpp"
	//
	#include <dinput.h>
	//
	#include <windows.h>
	#include <windowsx.h>
	// Types
	typedef CHAR NNChar;
	typedef FLOAT NNFloat;
	typedef INT NNInt;
	typedef UINT NNUInt;
	typedef LONG NNLong;
	typedef ULONG64 NNULong;
	typedef BYTE NNByte;
	typedef DirectX::SimpleMath::Vector2 NNVec2;
	typedef DirectX::SimpleMath::Vector3 NNVec3;
	typedef DirectX::SimpleMath::Vector4 NNVec4;
	typedef DirectX::SimpleMath::Matrix NNMat4;

	// Math Functions
	#define NNMat4Identity DirectX::SimpleMath::Matrix::Identity()
	#define NNRadians(fDegrees) DirectX::XMConvertToRadians(fDegrees)
	#define NNCreateTranslation(pos) DirectX::SimpleMath::Matrix::CreateTranslation(pos)
	#define NNCreateRotationX(mat, angle) mat * DirectX::SimpleMath::Matrix::CreateRotationX(angle)
	#define NNCreateRotationY(mat, angle) mat * DirectX::SimpleMath::Matrix::CreateRotationY(angle)
	#define NNCreateRotationZ(mat, angle) mat * DirectX::SimpleMath::Matrix::CreateRotationZ(angle)
	#define NNCreateScale(scale) DirectX::SimpleMath::Matrix::CreateScale(scale)
	#define NNCreateLookAt(position, target, up) DirectX::SimpleMath::Matrix::CreateLookAt(position, target, up)
	#define NNCreatePerspective(fov, ratio, nearplane, farplane) DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(fov, ratio, nearplane, farplane)
	#define NNCreateOrtho(left, right, bottom, top, nearplane, farplane) DirectX::SimpleMath::Matrix::CreateOrthographicOffCenter(left, right, bottom, top, nearplane, farplane)
	#define NNCross(vec1, vec2) vec1.Cross(vec2)
	#define NNNormalize(vec1) (vec1 / vec1.Length())
	#define NNMat4ValPtr(mat) mat.Transpose()
	#define NNMat4MulVec4(mat, vec) DirectX::SimpleMath::Vector4::Transform(vec, mat)
	//
	enum NNMouseButton {
		NN_MOUSE_BUTTON_LEFT, NN_MOUSE_BUTTON_MID, NN_MOUSE_BUTTON_RIGHT, NN_MOUSE_BUTTON_EXT
	};
	// 图元种类
	enum NNDrawMode {
		NN_POINT = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
		NN_LINE = D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		NN_LINE_STRIP = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
		NN_TRIANGLE = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		NN_TRIANGLE_STRIP = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
	};
	//
	enum NNTestFunc {
		NEVER = D3D11_COMPARISON_NEVER,
		LESS = D3D11_COMPARISON_LESS_EQUAL,
		EQUAL = D3D11_COMPARISON_EQUAL,
		LEQUAL = D3D11_COMPARISON_LESS_EQUAL,
		GREATER = D3D11_COMPARISON_GREATER,
		NOTEQUAL = D3D11_COMPARISON_NOT_EQUAL,
		GEQUAL = D3D11_COMPARISON_GREATER_EQUAL,
		ALWAYS = D3D11_COMPARISON_ALWAYS
	};
	//
	enum NNStencilOp {
		KEEP = D3D11_STENCIL_OP_KEEP,
		ZERO = D3D11_STENCIL_OP_ZERO,
		REPLACE = D3D11_STENCIL_OP_REPLACE,
		INCREASE = D3D11_STENCIL_OP_INCR_SAT,
		DECREASE = D3D11_STENCIL_OP_DECR_SAT,
		INVERT = D3D11_STENCIL_OP_INVERT,
		INCREASE_WRAP = D3D11_STENCIL_OP_INCR,
		DECREASE_WRAP = D3D11_STENCIL_OP_DECR
	};
	// 窗口类型
	#define NNWindow HWND
	// 键盘映射
	#define NNKeyMap(key) CONNECT2(DIK_, key)
#else
	#error Please define NENE_GL or NENE_DX to select which GAPI you want to Use!
#endif // Cross GAPI Platform

#endif // TYPES_H
