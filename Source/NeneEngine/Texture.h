/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef TEXTURE_H
#define TEXTURE_H

#include "Utils.h"
#include <memory>

//
//    Texture: Abstract Class for Texture Class
//

class Texture : public std::enable_shared_from_this<Texture> {
public:
	// 
	virtual void Use(const NNUInt& slot = 0) = 0;
	//
	static std::shared_ptr<NNByte[]> LoadImage(const NNChar* filepath, NNUInt& width, NNUInt& height, NNColorFormat &format);
};

#endif // TEXTURE_H
