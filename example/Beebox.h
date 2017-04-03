// Beebox.h
// example for beebox

#ifndef __BEEBOX_H__
#define __BEEBOX_H__

#include "LoopTask.h"
#include "Timer.h"
#include "Semaphore.h"

using namespace beebox;

class CBeebox : public sigslot::has_slots<>
{
public:
	CBeebox();
	~CBeebox();

	void run();
	void stop();
	void shot();
	void beWaiting();

	void onTask(void* data);
	void onTimer();

private:
	CLoopTask m_loopTask;
	CTimer m_timer;
	CSemaphore m_semaphore;
};

#endif
