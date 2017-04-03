
#include "Async.h"

namespace beebox
{

typedef struct _runtime_data_
{
	CAsync* async;
	void* arg;
}AsyncRuntimeData;

static void onAsync(uv_async_t* handle)
{
	AsyncRuntimeData* rData = (AsyncRuntimeData*)handle->data;

	CAsync* async = rData->async;
	async->doAsync(rData->arg);

	delete rData;
}

CAsync::CAsync(CLoop* loop)
{
	init(loop);
}

CAsync::~CAsync()
{
}

void CAsync::send(void* arg)
{
	AsyncRuntimeData* rData = new AsyncRuntimeData;
	rData->async = this;
	rData->arg = arg;

	m_async.data = (void*)rData;
	uv_async_send(&m_async);
}

void CAsync::init(CLoop* loop)
{
	uv_async_init(loop->get(), &m_async, onAsync);
}

} // namespace beebox
