
#include "Task.h"
#include "Thread.h"
#include "Semaphore.h"

namespace beebox
{

CTask::CTask()
{
	m_continue = true;

	m_thread = new CThread();
	m_thread->body.connect(this, &CTask::doWork);

	m_semaphore = new CSemaphore();
	m_enableBlock = false;
}

CTask::~CTask()
{
	m_thread->join();
	delete m_thread;
	delete m_semaphore;
}

void CTask::start()
{
	m_thread->create();
}

void CTask::stop()
{
	m_continue = false;
	m_thread->join();
}

void CTask::doWork()
{
	while (m_continue)
	{
		if (m_enableBlock)
		{
			m_semaphore->wait();
		}

		run();
	}
}

void CTask::blockEnable(bool enable)
{
	bool lastEnable = m_enableBlock;
	m_enableBlock = enable;

	if (lastEnable && !enable)
	{
		m_semaphore->post();
	}
}

void CTask::post()
{
	if (m_enableBlock)
	{
		m_semaphore->post();
	}
}

} // namespace beebox
