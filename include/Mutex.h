
#ifndef __MUTEX_H__
#define __MUTEX_H__

#include "libuv/uv.h"

namespace beebox
{

class CMutex
{
public:
	CMutex();
	~CMutex();

	void lock();
	void unlock();

private:
	uv_mutex_t m_mutex;
};

class CLock
{
public:
	CLock(CMutex& mutex);
	~CLock();

private:
	CMutex* m_mutex;
};

}

#endif // __MUTEX_H__
