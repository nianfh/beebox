
#include "LoopTask.h"
#include "Async.h"

namespace beebox
{

CLoopTask::CLoopTask() : m_async(&m_loop)
{
	m_async.doAsync.connect(this, &CLoopTask::onAsync);
	blockEnable(false);
}

CLoopTask::~CLoopTask()
{
}

CLoop* CLoopTask::getLoop()
{
	return &m_loop;
}

void CLoopTask::active(void* data)
{
	m_async.send(data);
}

void CLoopTask::stop()
{
	m_loop.stop();
	CTask::stop();
}

void CLoopTask::runTask(void* data)
{

}

void CLoopTask::run()
{
	m_loop.start();
}

void CLoopTask::onAsync(void* data)
{
	runTask(data);
	doTask(data);
}

} // namespace beebox
