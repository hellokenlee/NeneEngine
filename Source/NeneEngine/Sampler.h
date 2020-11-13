/*Copyright reserved by KenLee@2020 hellokenlee@163.com*/
#ifndef SAMPLER_H
#define SAMPELR_H

#include <memory>
#include "Types.h"


class Sampler
{
public:
	// 
	static std::shared_ptr<Sampler> Create(const NNUInt& min_filter, const NNUInt& mag_filter, const NNUInt& wrap_u, const NNUInt& wrap_v, const NNUInt& wrap_w);

public:
	//
	~Sampler();

public:
	NNUInt m_min_filter, m_mag_filter;
	NNUInt m_wrap_u, m_wrap_v, m_wrap_w;

private:
	// 
	Sampler() = default;
	Sampler(const Sampler& rhs) = delete;
	Sampler& operator=(const Sampler& rhs) = delete;
};

#endif // SAMPLER_H