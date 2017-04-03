
#ifndef __HTTP_SERVER_H__
#define __HTTP_SERVER_H__

#include "HttpConnection.h"
#include "TcpServer.h"
#include "File.h"

#include <map>

namespace beebox
{

using namespace std;

enum route_type
{
	RouteGet,
	RoutePost,
	RouteOptions,
};

class CRouter;

class CHttpServer : public sigslot::has_slots<>
{
public:
	CHttpServer(int port, CLoop* loop = NULL);
	virtual ~CHttpServer();

	void run();

	void setRouter(CRouter* router);

	void onClientConnected(CTcpSocket* tcp);
	void onClientDisconnected(CHttpConnection* httpConnection);
	void onRoute(CHttpConnection* httpConnection);

private:
	int m_port;
	CLoop* m_loop;
	CTcpServer* m_tcpServer;

	CRouter* m_router;

	list<CHttpConnection*> m_httpConnectionList;

	bool m_uploadFileEnable;
};

class CRouter : public sigslot::has_slots<>
{
public:
	CRouter();
	virtual ~CRouter();

private:
	typedef sigslot::signal1<CHttpConnection*> RouteProc;
	typedef map<string, RouteProc*> RouterMap;

public:
	template<class routerType>
	void addRoute(int routeType, string path, routerType* router, void (routerType::*route)(CHttpConnection*))
	{
		RouteProc* proc = new RouteProc;
		proc->connect(router, route);

		if (routeType == RouteGet)
		{
			m_getMap[path] = proc;
		}
		else if (routeType == RoutePost)
		{
			m_postMap[path] = proc;
		}
		else if (routeType == RouteOptions)
		{
			m_optionsMap[path] = proc;
		}
	};

	void setFileRouteRootDir(string diractory);
	void setUploadTempFilesDir(string diractory);
	string getUploadTempFilesDir();
	bool routeGet(CHttpConnection* httpConnection);
	bool routePost(CHttpConnection* httpConnection);
	bool routeOptions(CHttpConnection* httpConnection);

private:
	bool routeFile(CHttpConnection* httpConnection);
	bool route(RouterMap& routeMap, CHttpConnection* httpConnection);

private:
	RouterMap m_getMap;
	RouterMap m_postMap;
	RouterMap m_optionsMap;

	string m_fileRouteDir;
	string m_fileTempDir;
};

} // namespace beebox

#endif // __HTTP_SERVER_H__
