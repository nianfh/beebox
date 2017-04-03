
#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include "libuv/uv.h"

namespace beebox
{

class CSemaphore
{
public:
	CSemaphore();
	~CSemaphore();

	void wait();
	void post();

private:
	uv_sem_t m_sem;
};

}

#endif // __SEMAPHORE_H__
