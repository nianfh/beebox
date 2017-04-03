
#include "Thread.h"
#include "libuv/uv.h"

namespace beebox
{

void threadBody(void* arg)
{
	CThread* thread = (CThread*)arg;
	thread->body();
}

CThread::CThread()
{
}

CThread::~CThread()
{
}

void CThread::create()
{
	uv_thread_create(&m_threadId, threadBody, this);
}

void CThread::join()
{
	uv_thread_join(&m_threadId);
}

} //  namespace beebox
