
#ifndef __TIMER_H__
#define __TIMER_H__

#include "Loop.h"
#include "sigslot.h"

namespace beebox
{

class CTimer
{
public:
	CTimer(CLoop* loop);
	virtual ~CTimer();

	void start(long long repeat, long long delay=0);
	void stop();
	bool again();
	void setRepeat(long long repeat);

	sigslot::signal0<> doTimer;

private:
	uv_timer_t m_timer;
};

}

#endif // __TIMER_H__
