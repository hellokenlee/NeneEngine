/*Copyright reserved by KenLee@2018 hellokenlee@163.com*/

#include "IO.h"
#include "Debug.h"
#include <fstream>
#include <sstream>

using namespace std;

string IO::ReadFile(const NNChar *filepath) {
	//
	checkFileExist(filepath);
	//
	string sourceString = "";
	ifstream inFile;
	inFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	// 读取文件
	try {
		//从文件读入
		inFile.open(filepath);
		std::stringstream shaderSourceStream;
		shaderSourceStream << inFile.rdbuf();
		inFile.close();
		sourceString = shaderSourceStream.str();
	} catch (std::ifstream::failure e) {
		//处理异常
		dLog("[Error]: Fail to read source file(%s)!\n", filepath);
		dLog("%s\n\n", e.what());
	}
	return sourceString;
}

void IO::SaveFile(const NNChar* filepath, const string &content) {
	ofstream outFile;
	//
	try {
		//
		outFile.open(filepath);
		outFile.write(content.c_str(), content.size());
		outFile.close();
	} catch (std::ofstream::failure e) {
		//处理异常
		dLog("[Error]: Fail to write target file(%s)!\n", filepath);
		dLog("%s\n\n", e.what());
	}
}

#ifdef NENE_DX

string IO::WS2S(const wstring& origin) {
	// 获取原来的长度
	const size_t originLen = origin.length();
	// 计算宽字符串长度
	const int resLen = WideCharToMultiByte(CP_UTF8, 0, origin.c_str(), originLen, NULL, 0, NULL, NULL);
	// 新字符串缓冲
	char* resBuff = new char[resLen];
	// 拷贝
	WideCharToMultiByte(CP_UTF8, 0, origin.c_str(), originLen, resBuff, resLen, NULL, NULL);
	// 实例化
	string res(resBuff, resLen);
	// 
	delete[] resBuff;
	//
	return res;
}

wstring IO::S2WS(const string& origin) {
	const size_t originLen = origin.length();
	const int resLen = MultiByteToWideChar(CP_UTF8, 0, origin.c_str(), originLen, 0, 0);
	wchar_t* resBuff = new wchar_t[resLen];
	MultiByteToWideChar(CP_UTF8, 0, origin.c_str(), originLen, resBuff, resLen);
	wstring res(resBuff, resLen);
	delete[] resBuff;
	return res;
}

#endif NENE_DX