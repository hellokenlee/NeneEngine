/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#include "Debug.h"
#include "Sampler.h"

using namespace std;

Sampler::~Sampler()
{}

shared_ptr<Sampler> Sampler::Create(const NNUInt& min_filter, const NNUInt& mag_filter, const NNUInt& wrap_u, const NNUInt& wrap_v, const NNUInt& wrap_w)
{
	Sampler* result = new Sampler();
	result->m_min_filter = min_filter;
	result->m_mag_filter = mag_filter;
	result->m_wrap_u = wrap_u;
	result->m_wrap_v = wrap_v;
	result->m_wrap_w = wrap_w;
	return shared_ptr<Sampler>(result);
}
