/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef NENE_CB_H
#define NENE_CB_H

#include "ConstantBuffer.h"

//
//    NeneCB: A singleton to manage constant buffer inside the engine
//

// 系统常量缓冲位置
enum NNConstantBufferSlot {
	PER_FRAME_SLOT = 0,
	PER_OBJECT_SLOT = 1,
	NNConstantBufferSoltNum
};

// 每帧更新的常量缓冲结构
struct PerFrameCBDS {
	//
	NNMat4 view;
	NNMat4 projection;
	NNVec3 camera_position;
	NNFloat _padding;
	//
	NNFloat curr_time, sin_time, cos_time;
	NNFloat _padding2;
	//
	NNMat4 shadowlight_space;
};

// 每物体更新的常量缓冲结构
struct PerObjectCBDS {
	NNMat4 model;
};

class NeneCB {
public:
	// 获取单例
	static NeneCB& Instance();
	// 每帧更新的常量缓冲
	inline ConstantBuffer<PerFrameCBDS>& PerFrame() { return m_per_frame; };
	// 每物体更新的常量缓冲
	inline ConstantBuffer<PerObjectCBDS>& PerObject() { return m_per_object; };
protected:
	//
	ConstantBuffer<PerFrameCBDS> m_per_frame;
	ConstantBuffer<PerObjectCBDS> m_per_object;
private:
	// 构造
	NeneCB();
	NeneCB(const NeneCB& rhs);
	NeneCB& operator = (const NeneCB& rhs);
};

#endif // NENE_CB_H