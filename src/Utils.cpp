
#include "Utils.h"
#include "libuv/uv.h"

#include <stdarg.h>
#include <math.h>
#include <sstream>
#include <stdlib.h>

#ifdef LINUX
#define _vsnprintf vsnprintf
#define _popen popen
#define _pclose pclose
#endif

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace beebox
{

template <typename T>
void stringConvert(string& in, T& out)
{
	stringstream ss;
	ss << in;

	ss >> out;
}

CUtils::CUtils()
{
}

CUtils::~CUtils()
{
}

string CUtils::currentPath()
{
	char buffer[512] = { 0 };
	size_t size;
	uv_exepath(buffer, &size);

	return string(buffer);
}

std::string CUtils::dirOfFilePath(string filePath)
{
	size_t pos = filePath.rfind('/');
	if (pos != string::npos)
	{
		return filePath.substr(0, pos);
	}

	return filePath;
}

void CUtils::sleep(unsigned int milliseconds)
{
#ifdef WIN32
	::Sleep(milliseconds);
#else
	::usleep(milliseconds * 1000);
#endif
}

void CUtils::stringSplit(const string& s, vector<string>& v, const string& c)
{
	string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;

	while (string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}

	if (pos1 != s.length())
	{
		v.push_back(s.substr(pos1));
	}
}

void CUtils::stringFormat(string& s, const char* format, ...)
{
	va_list arglist;
	va_start(arglist, format);
	int size = _vsnprintf(NULL, 0, format, arglist) + 2;

	char stackBuffer[1024] = { 0 };
	char* buffer = stackBuffer;
	if (size > 1024)
	{
		buffer = new char[size];
	}

	va_start(arglist, format);
	int ret = _vsnprintf(buffer, size, format, arglist);

	va_end(arglist);

	s = ret < 0 ? "" : buffer;

	if (size > 1024)
	{
		delete [] buffer;
	}
}


void CUtils::stringReplace(string& s, string oldStr, string newStr)
{
	for (string::size_type pos(0); pos != string::npos; pos += newStr.length())
	{
		if ((pos = s.find(oldStr, pos)) != string::npos)
		{
			s.replace(pos, oldStr.length(), newStr);
		}
		else
		{
			break;
		}
	}
}

int CUtils::stringToInt(string& s)
{
	int result;
	stringConvert(s, result);

	return result;
}

float CUtils::stringToFloat(string& s)
{
	float result;
	stringConvert(s, result);

	return result;
}

double CUtils::stringToDouble(string& s)
{
	double result;
	stringConvert(s, result);

	return result;
}

bool CUtils::system(string& cmd)
{
	printf("system:%s\n", cmd.c_str());
#ifdef WIN32
	return ::system(cmd.c_str()) == 0 ? true : false;
#else
	pid_t status;

	status = ::system(cmd.c_str());

	if (-1 == status)
	{
		printf("system error!");
	}
	else
	{
		if (WIFEXITED(status))
		{
			if (0 == WEXITSTATUS(status))
			{
				return true;
			}
			else
			{
				printf("run shell script fail, script exit code: %d\n", WEXITSTATUS(status));
			}
		}
		else
		{
			printf("exit status = [%d]\n", WEXITSTATUS(status));
		}
	}

	return false;
#endif
}

std::string CUtils::excuteCmd(string& cmd)
{
	FILE* f = _popen(cmd.c_str(), "r");
	char buffer[1024] = { 0 };
	fread(buffer, 1024, 1, f);
	printf("%s\n", buffer);
	_pclose(f);

	return string(buffer);
}

} // namespace beebox
