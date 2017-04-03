#include "Beebox.h"
#include "Utils.h"
#include "Date.h"

CBeebox::CBeebox():
	m_timer(m_loopTask.getLoop())
{
	m_loopTask.doTask.connect(this, &CBeebox::onTask);
	m_timer.doTimer.connect(this, &CBeebox::onTimer);
}

CBeebox::~CBeebox()
{
}

void CBeebox::run()
{
	printf("Beebox start at %s\n", CDate().toString().c_str());
	m_loopTask.start();
	m_timer.start(1000, 2000);
}

void CBeebox::stop()
{
	printf("timer stop...\n");
	m_timer.stop();

	printf("Beebox stop...\n");
	m_loopTask.stop();

	m_semaphore.post();
}

void CBeebox::shot()
{
	m_loopTask.active();
}

void CBeebox::beWaiting()
{
	m_semaphore.wait();
}

void CBeebox::onTask(void* data)
{
	static int shotCount = 0;
	printf("Beebox shot[%d]\n",  ++shotCount);
}

void CBeebox::onTimer()
{
	static int count = 0;
	printf("timer timeout[%d]...\n", ++count);

	if (count >= 3) {
		stop();
	}
}
