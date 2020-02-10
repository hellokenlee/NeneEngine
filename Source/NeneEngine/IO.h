/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/
#ifndef IO_H
#define IO_H

#include "Types.h"
#include <string>

//
//    IO: File's Reading and Writing Class
//

class IO {
public:
	// 读取文件
	static std::string ReadFile(const NNChar* filepath);
	// 写入文件
	static void SaveFile(const NNChar* filepath, const std::string &content);
#ifdef NENE_DX
	// 宽字符和字符转换
	static std::string WS2S(const std::wstring&);
	static std::wstring S2WS(const std::string&);
#endif
};

#endif // IO_H