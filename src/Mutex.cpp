
#include "Mutex.h"

namespace beebox
{

CMutex::CMutex()
{
	uv_mutex_init(&m_mutex);
}

CMutex::~CMutex()
{
	uv_mutex_destroy(&m_mutex);
}

void CMutex::lock()
{
	uv_mutex_lock(&m_mutex);
}

void CMutex::unlock()
{
	uv_mutex_unlock(&m_mutex);
}


// CLock
CLock::CLock(CMutex& mutex)
{
	m_mutex = &mutex;
	m_mutex->lock();
}

CLock::~CLock()
{
	m_mutex->unlock();
}

} // namespace beebox
