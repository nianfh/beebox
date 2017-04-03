
#include "Timer.h"

namespace beebox
{

static void timerBody(uv_timer_t* handle)
{
	CTimer* timer = (CTimer*)handle->data;
	timer->doTimer();
}

CTimer::CTimer(CLoop* loop)
{
	uv_timer_init(loop->get(), &m_timer);
	m_timer.data = (void*)this;
}

CTimer::~CTimer()
{
}

void CTimer::start(long long repeat, long long delay)
{
	uv_timer_start(&m_timer, timerBody, delay, repeat);
}

void CTimer::stop()
{
	uv_timer_stop(&m_timer);
}

bool CTimer::again()
{
	return uv_timer_again(&m_timer) == UV_EINVAL ? false : true;
}

void CTimer::setRepeat(long long repeat)
{
	uv_timer_set_repeat(&m_timer, repeat);
}

} // namespace beebox
