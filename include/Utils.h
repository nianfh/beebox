
#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <string>
#include <vector>

using namespace std;

/////////////////////////////////////////////////
////////////// SINGLETON_IMPLEMENT //////////////
// remember to make the constructor and destrutor private
#define SINGLETON_IMPLEMENT(T)          \
	static T* instance()                    \
	{                                   \
		static T* _instance = NULL;     \
		if( NULL == _instance)          \
		{                               \
			_instance = new T;			\
		}                               \
		return _instance;               \
	};
////////////////////////////////////////////////

#define ARRAY_SIZE(array, type) (sizeof(array)/sizeof(type))

namespace beebox
{

class CUtils
{
public:
	CUtils();
	~CUtils();

	static string currentPath();
	static string dirOfFilePath(string filePath);
	static void sleep(unsigned int milliseconds);

	static void stringSplit(const string& s, vector<std::string>& v, const string& c);
	static void stringFormat(string& s, const char * format, ...);
	static void stringReplace(string& s, string oldStr, string newStr);
	static int stringToInt(string& s);
	static float stringToFloat(string& s);
	static double stringToDouble(string& s);

	static bool system(string& cmd);
	static string excuteCmd(string& cmd);
};

}

#endif // __UTILS_H__
