/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef PARTICLE_SYSTEM
#define PARTICLE_SYSTEM

#include "Utils.h"
#include "Shader.h"
#include "Camera.h"
#include "ConstantBuffer.h"

template<typename ParticleData, typename EmitterData>
class ParticleSystem {
public:
	//
	ParticleSystem(const NNUInt particleNum);
	~ParticleSystem();
	//
	void set();
	//
	void reset();
	//
	void Update(std::shared_ptr<Shader> pShader, const NNFloat& dt);
	void Draw(std::shared_ptr<Shader> pShader, std::shared_ptr<Camera> pCam);
private:
	NNUInt mParticleNum;
	NNUInt mParticleSize;
	NNUInt mVBOffset;
	ConstantBuffer<EmitterData> mEmitterCB;

#if defined NENE_DX

#elif defined NENE_GL

#endif
private:
	//
	ParticleSystem();
	ParticleSystem(const ParticleSystem &otehr);
	ParticleSystem& operator=(const ParticleSystem& rhs);
};

//
#include "ParticleSystem_DX.inl"
//
#include "ParticleSystem_GL.inl"

#endif // PARTICLE_SYSTEM
