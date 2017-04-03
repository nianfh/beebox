
#ifndef __ASYNC_H__
#define __ASYNC_H__

#include "Loop.h"
#include "sigslot.h"

namespace beebox
{

class CAsync
{
public:
	CAsync(CLoop* loop);
	~CAsync();

	sigslot::signal1<void*> doAsync;
	void send(void* arg = NULL);

private:
	void init(CLoop* loop);

private:
	uv_async_t m_async;
};

}

#endif // __ASYNC_H__
