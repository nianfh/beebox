
#include "File.h"
#include "Utils.h"

#include <string.h>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

namespace beebox
{

CFile::CFile()
{
	m_file = NULL;
	m_buffer = NULL;
	m_size = 0;
}

CFile::CFile(string path)
{
	m_file = NULL;
	m_buffer = NULL;
	m_size = 0;

	open(path, READ_WRITE);
}

CFile::~CFile()
{
	if(m_file)
	{
		close();
	}

	if(m_buffer)
	{
		unload();
	}
}

bool CFile::open(string path, unsigned int flag)
{
	m_path = path;

	string flagStr;

	string typeStr = "b";
	if (flag & TEXT)
	{
		typeStr = "t";
	}

	switch (flag & 0xf) 
	{
	case READ:
		flagStr = "r";
		flagStr += typeStr;
		break;
	case WRITE:
		flagStr = "w";
		flagStr += typeStr;
		break;
	case READ_WRITE:
		if(flag & CREATE)
		{
			flagStr = "w";
			flagStr += typeStr;
			flagStr += '+';
		}
		else
		{
			flagStr = "r";
			flagStr += typeStr;
			flagStr += '+';
		}
	}

	m_file = fopen(path.c_str(), flagStr.c_str());

	if(!m_file)
	{
		return false;
	}

	return true;
}

bool CFile::open(unsigned int flag /*= READ*/)
{
	return open(m_path, flag);
}

void CFile::close()
{
	if(!m_file)
	{
		return;
	}

	fclose(m_file);

	m_file = NULL;
}

unsigned int CFile::read(void* buffer, unsigned int size)
{
	if(!m_file)
	{
		return 0;
	}

	return fread(buffer, 1, size, m_file);
}

unsigned int CFile::write(void* buffer, unsigned int size)
{
	if(!m_file)
	{
		return 0;
	}

	return fwrite(buffer, 1, size, m_file);
}

void CFile::flush()
{
	if(!m_file)
	{
		return;
	}

	fflush(m_file);
}

unsigned int CFile::seek(long offset, unsigned int from)
{
	if(!m_file)
	{
		return 0;
	}

	int flag = 0;
	switch(from)
	{
	case BEGIN:
		flag = SEEK_SET;
		break;

	case CURRENT:
		flag = SEEK_CUR;
		break;

	case END:
		flag = SEEK_END;
		break;
	}

	if(!fseek(m_file, offset, flag))
	{
		return getPosition();
	};

	return 0;
}

unsigned int CFile::getPosition()
{
	if(!m_file)
	{
		return 0;
	}

	int pos = ftell(m_file);
	if(pos >= 0)
	{
		return pos;
	}
	return 0;
}

unsigned int CFile::getSize()
{
	if(!m_buffer)
	{
		unsigned int lastPos = getPosition();
		if(seek(0, END))
		{
			m_size = getPosition();
		};
		seek(lastPos, BEGIN);
	}

	return m_size;
}

std::string CFile::getName()
{
	string fullName = getFullName();
	
	size_t pos = fullName.rfind('.');
	if (pos != string::npos)
	{
		return fullName.substr(0, pos);
	}

	return fullName;
}

std::string CFile::getFullName()
{
	size_t pos = m_path.rfind('/');
	if (pos != string::npos)
	{
		return m_path.substr(pos + 1);
	}

	return m_path;
}

std::string CFile::getExtension()
{
	size_t pos = m_path.rfind('.');
	if (pos != string::npos)
	{
		return m_path.substr(pos + 1);
	}
	
	return "";
}

std::string CFile::getPath()
{
	size_t pos = m_path.rfind('/');
	if (pos != string::npos)
	{
		return m_path.substr(0, pos);
	}

	return m_path;
}

std::string CFile::getFullPath()
{
	return m_path;
}

char* CFile::load(string path)
{
	unsigned int ret;
	bool opened = false;
	if (!isOpened())
	{
		if (open(path.c_str(), READ))
		{
			opened = true;
		}
		else
		{
			return NULL;
		}
	}

	if (!m_buffer)
	{
		m_size = getSize();
		if(m_size)
		{
			m_buffer = new char[m_size +1];
			memset(m_buffer, 0, m_size +1);
			if(m_buffer)
			{
				seek(0, BEGIN);
				ret = read(m_buffer, m_size);
				if(!ret)
				{
					printf("CFile::read error :fileName %s \n",path.c_str());
					return NULL;
				}
			}
		}
	}

	if (opened)
	{
		close();
	}	

	return m_buffer;
}

void CFile::unload()
{
	if(m_buffer)
	{
		delete [] m_buffer;
		m_buffer = NULL;
	}
}

bool CFile::rename(std::string path, std::string newName)
{
	return ::rename(path.c_str(), newName.c_str()) == 0;
}

bool CFile::remove(std::string path)
{
	return ::remove(path.c_str()) == 0;
}

bool CFile::makeDir(std::string path)
{
	if (!access(path.c_str(), 0))
	{
		return true; // ����Ŀ¼���ڣ�ֱ�ӷ��سɹ�
	}

#ifdef WIN32
	string cmd("md ");
	
	string::iterator it = path.begin();
	while (it != path.end())
	{
		if (*it == '/')
		{
			*it = '\\';
		}

		it++;
	}
#else
	string cmd("mkdir ");
#endif

	cmd += path;
	return CUtils::system(cmd);
}

bool CFile::removeDir(std::string path)
{
#ifdef WIN32
	string cmd("rd ");

	string::iterator it = path.begin();
	while (it != path.end())
	{
		if (*it == '/')
		{
			*it = '\\';
		}

		it++;
	}
#else
	string cmd("rmdir ");
#endif

	cmd += path;
	return CUtils::system(cmd);
}

} // namespace beebox
