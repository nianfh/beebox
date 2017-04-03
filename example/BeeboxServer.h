// BeeboxServer.h
// example for HTTP Server

#ifndef __BEEBOX_SERVER_H__
#define __BEEBOX_SERVER_H__

#include "LoopTask.h"
#include "HttpServer.h"
#include "Semaphore.h"

using namespace beebox;

class CBeeboxServer : public sigslot::has_slots<>
{
public:
	CBeeboxServer();
	~CBeeboxServer();

	void run();
	void stop();

	void hello(CHttpConnection* conn);
	void end(CHttpConnection* conn);
	void login(CHttpConnection* conn);

private:
	CLoopTask m_loopTask;
	CSemaphore m_s;

	CRouter m_httpRouter;
	CHttpServer m_httpServer;
};

#endif
