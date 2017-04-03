
#ifndef __LOOP_H__
#define __LOOP_H__

#include "libuv/uv.h"

namespace beebox
{

class CLoop
{
public:
	CLoop()
	{
		m_loop = uv_loop_new();
		uv_loop_init(m_loop);
	};

	~CLoop()
	{
		delete m_loop;
	};

	void start()
	{
		uv_run(m_loop, UV_RUN_DEFAULT);
	};

	void stop()
	{
		uv_stop(m_loop);
	};

	uv_loop_t* get()
	{
		return m_loop;
	};

private:
	uv_loop_t* m_loop;
};

}

#endif // __LOOP_H__
