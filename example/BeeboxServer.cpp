
#include "BeeboxServer.h"
#include "Utils.h"

#define HTTP_PORT 8080

CBeeboxServer::CBeeboxServer():
	m_httpServer(HTTP_PORT, m_loopTask.getLoop())
{
	m_httpRouter.setFileRouteRootDir("./");
	m_httpRouter.addRoute(RouteGet, "/", this, &CBeeboxServer::hello);
	m_httpRouter.addRoute(RouteGet, "/end", this, &CBeeboxServer::end);
	m_httpRouter.addRoute(RoutePost, "/login", this, &CBeeboxServer::login);
	m_httpServer.setRouter(&m_httpRouter);
}

CBeeboxServer::~CBeeboxServer()
{
}

void CBeeboxServer::run()
{
	printf("Beebox HTTP Server start port:[%d]\n", HTTP_PORT);
	m_loopTask.start();

	m_s.wait();
}

void CBeeboxServer::stop()
{
	printf("Beebox HTTP Server stop...\n");
	m_loopTask.stop();

	m_s.post();
}

void CBeeboxServer::hello(CHttpConnection* conn)
{
	string method = conn->getRequestMethod();
	string path = conn->getRequestPath();
	printf("Method=%s, Path=%s\n", method.c_str(), path.c_str());

	conn->setResponseStatus(HTTP_STATUS_200);
	conn->setResponseHeader("Connection", "Keep-Alive");
	conn->setResponseHeader("Content-Type", "text/json");
	conn->sendResponseSimple("{\"name\": \"Beebox Server\", \"content\": \"Hi\"}");
}

void CBeeboxServer::end(CHttpConnection* conn)
{
	string method = conn->getRequestMethod();
	string path = conn->getRequestPath();
	printf("Method=%s, Path=%s\n", method.c_str(), path.c_str());

	conn->setResponseStatus(HTTP_STATUS_200);
	conn->setResponseHeader("Connection", "Close");
	conn->setResponseHeader("Content-Type", "text/json");
	conn->sendResponseSimple("{\"result\": true, \"message\": \"success\"}");

	CUtils::sleep(1000);

	stop();
}

void CBeeboxServer::login(CHttpConnection* conn)
{
	string method = conn->getRequestMethod();
	string path = conn->getRequestPath();
	printf("Method=%s, Path=%s\n", method.c_str(), path.c_str());

	// do something...

	conn->setResponseStatus(HTTP_STATUS_200);
	conn->setResponseHeader("Connection", "Keep-Alive");
	conn->setResponseHeader("Content-Type", "text/json");
	conn->sendResponseSimple("{\"result\": true}");
}
