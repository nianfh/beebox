
#include "Semaphore.h"

namespace beebox
{

CSemaphore::CSemaphore()
{
	uv_sem_init(&m_sem, 0);
}

CSemaphore::~CSemaphore()
{
	uv_sem_destroy(&m_sem);
}

void CSemaphore::wait()
{
	uv_sem_wait(&m_sem);
}

void CSemaphore::post()
{
	uv_sem_post(&m_sem);
}

} // namespace beebox

