
#ifndef __FILE_H__
#define __FILE_H__

#include <stdio.h>
#include <string>

using namespace std;

namespace beebox
{

class CFile
{
public:
	CFile();
	CFile(string path);
	~CFile();

	enum openFlag
	{
		READ       = 0x0000, // 为读打开一个文件，如果文件不存在或无法打开则失败
		WRITE      = 0x0001, // 为写创建一个空文件，如果文件存在，其内容将被清空
		READ_WRITE = 0x0002, // 为读写打开一个文件，如果文件不存在或无法打开则失败

		CREATE     = 0x0010, // 如果文件不存在，自动创建文件，和READ_WRITE配合使用
		TEXT       = 0x0020, // 创建或者打开一个文本文件，默认是二进制文件
	};

	enum seekFlag
	{
		BEGIN   = 0x0001,
		CURRENT = 0x0002,
		END     = 0x0004,
	};

	bool open(string path, unsigned int flag = READ);
	bool open(unsigned int flag = READ);
	void close();
	bool isOpened(){return m_file ? true : false;};

	unsigned int read(void* buffer, unsigned int bufferSize);
	unsigned int write(void* buffer, unsigned int bufferSize);

	void flush();
	unsigned int seek(long offset, unsigned int from);
	unsigned int getPosition();
	unsigned int getSize();
	string getName();
	string getFullName();
	string getExtension();
	string getPath();
	string getFullPath();

	char* load(string path = "");
	void unload();

	static bool rename(std::string path, std::string newName);
	static bool remove(std::string path);
	static bool makeDir(std::string path);
	static bool removeDir(std::string path);

private:
	FILE* m_file;
	char* m_buffer;

	unsigned int m_size;
	string m_path;
};

}

#endif // __FILE_H__
