
#ifndef __TASK_H__
#define __TASK_H__

#include "sigslot.h"

namespace beebox
{

class CThread;
class CSemaphore;

class CTask : public sigslot::has_slots<>
{
public:
	CTask();
	virtual ~CTask();

	virtual void start();
	virtual void stop();

	virtual void run(){};
	virtual void post();

	void blockEnable(bool enable);

private:
	void doWork();

	CThread* m_thread;
	CSemaphore* m_semaphore;

	volatile bool m_continue;
	volatile bool m_enableBlock;
};

}

#endif // __TASK_H__
