/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef TEXTURE_3D_H
#define TEXTURE_3D_H

#include <vector>
#include "Texture.h"

class Texture3DImpl;

//
//
//

class Texture3D : public Texture
{
public:
	//
	~Texture3D();
	//
	static std::shared_ptr<Texture3D> Create(const std::vector<std::string>& filepaths);
	static std::shared_ptr<Texture3D> Create(const std::vector<std::vector<std::string>>& mipmapfilepaths);
	//
	virtual void Use(const NNUInt& slot = 0);
private:
	Texture3DImpl* m_impl;
private:
	Texture3D() = default;
	Texture3D(const Texture3D& rhs) = delete;
	Texture3D& operator=(const Texture3D& rhs) = delete;
};

#endif // TEXTURE_3D_H
