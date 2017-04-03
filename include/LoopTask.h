
#ifndef __LOOP_TASK_H__
#define __LOOP_TASK_H__

#include "Task.h"
#include "Loop.h"
#include "Async.h"

namespace beebox
{

class CLoopTask : public CTask
{
public:
	CLoopTask();
	virtual ~CLoopTask();

	sigslot::signal1<void*> doTask;

	CLoop* getLoop();
	void active(void* data = NULL);

	virtual void stop();
	virtual void runTask(void* data);

private:
	void run();
	void onAsync(void* data);

private:
	CLoop m_loop;
	CAsync m_async;
};

}

#endif // __LOOP_TASK_H__
