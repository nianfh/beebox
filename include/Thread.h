
#ifndef __THREAD_H__
#define __THREAD_H__

#include "sigslot.h"
#include "libuv/uv.h"

namespace beebox
{

class CThread
{
public:
	CThread();
	virtual ~CThread();

	void create();
	void join();

	sigslot::signal0<> body;

private:
	uv_thread_t m_threadId;
};

}

#endif // __THREAD_H__
