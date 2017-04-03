
#include "HttpServer.h"
#include "Utils.h"
#include "Date.h"

namespace beebox
{

CHttpServer::CHttpServer(int port, CLoop* loop)
{
	m_port = port;

	m_loop = loop;
	if (m_loop == NULL)
	{
		m_loop = new CLoop;
	}

	m_tcpServer = new CTcpServer(m_loop, port);
	m_tcpServer->onConnected.connect(this, &CHttpServer::onClientConnected);

	m_uploadFileEnable = false;
}

CHttpServer::~CHttpServer()
{

}

void CHttpServer::run()
{
	printf("Http Server Port: %d, Listening...\n", m_port);
	m_loop->start();
}

void CHttpServer::setRouter(CRouter* router)
{
	m_router = router;
	string tempDir = m_router->getUploadTempFilesDir();
	if (tempDir.size() > 1)
	{
		m_uploadFileEnable = true;
	}
}

void CHttpServer::onClientConnected(CTcpSocket* tcp)
{
	printf("CHttpServer::onClientConnected\n");
	CHttpConnection* httpConnection = new CHttpConnection(tcp);
	httpConnection->doRoute.connect(this, &CHttpServer::onRoute);
	httpConnection->doDisconnect.connect(this, &CHttpServer::onClientDisconnected);
	if (m_uploadFileEnable)
	{
		httpConnection->setUploadingCacheFileDir(m_router->getUploadTempFilesDir());
	}
	httpConnection->start();

	// delete useless http connection
	for (list<CHttpConnection*>::iterator it = m_httpConnectionList.begin();
		it != m_httpConnectionList.end();)
	{
		delete *it;
		it = m_httpConnectionList.erase(it);
	}
}

void CHttpServer::onClientDisconnected(CHttpConnection* httpConnection)
{
	m_httpConnectionList.push_back(httpConnection);
}

void CHttpServer::onRoute(CHttpConnection* httpConnection)
{
	string method = httpConnection->getRequestMethod();
	string path = httpConnection->getRequestPath();

	bool haveRouted = false;
	if (method == "GET")
	{
		haveRouted = m_router->routeGet(httpConnection);
	}
	else if (method == "POST")
	{
		haveRouted = m_router->routePost(httpConnection);
	}
	else if (method == "OPTIONS")
	{
		haveRouted = m_router->routeOptions(httpConnection);
	}

	if (!haveRouted)
	{
		if (path == "/")
		{
			httpConnection->setResponseHeader("Connection", "Close");
			httpConnection->sendResponseSimple("Welcome to Beebox Server 1.0\n");
		}
		else
		{
			httpConnection->setResponseStatus(HTTP_STATUS_404);
			httpConnection->setResponseHeader("Connection", "Close");
			httpConnection->sendResponseSimple(HTTP_STATUS_404);
		}
	}
}

/************************* CRouter *************************/
CRouter::CRouter()
{

}

CRouter::~CRouter()
{

}

void CRouter::setFileRouteRootDir(string diractory)
{
	m_fileRouteDir = diractory;
}

void CRouter::setUploadTempFilesDir(string diractory)
{
	m_fileTempDir = diractory;
}

std::string CRouter::getUploadTempFilesDir()
{
	return m_fileTempDir;
}

bool CRouter::routeGet(CHttpConnection* httpConnection)
{
	return route(m_getMap, httpConnection);
}

bool CRouter::routePost(CHttpConnection* httpConnection)
{
	return route(m_postMap, httpConnection);
}

bool CRouter::routeOptions(CHttpConnection* httpConnection)
{
	return route(m_optionsMap, httpConnection);
}

bool CRouter::routeFile(CHttpConnection* httpConnection)
{
	if (m_fileRouteDir.empty())
	{
		return false;
	}

	string path = httpConnection->getRequestPath();
	if (path == "/")
	{
		path += "index.html";
	}
	string filePath = m_fileRouteDir + path;

	CFile file;
	if (file.open(filePath))
	{
		char* content = file.load();
		int contentLen = file.getSize();
		if (content)
		{
			httpConnection->setResponseHeader("Content-Type",
				CHttpConnection::getContentType(file.getExtension()));
			httpConnection->sendResponseSimple(content, contentLen);

			return true;
		}
	}

	return false;
}

bool CRouter::route(RouterMap& routeMap, CHttpConnection* httpConnection)
{
	bool ret = false;

	string path = httpConnection->getRequestPath();
	RouterMap::iterator it = routeMap.find(path);
	if (it != routeMap.end())
	{
		(it->second)->emit(httpConnection);
		ret = true;
	}
	else
	{
		it = routeMap.find("/*");
		if (it != routeMap.end())
		{
			(it->second)->emit(httpConnection);
			ret = true;
		}
		else
		{
			ret = routeFile(httpConnection);
		}
	}

	return ret;
}

} // namespace beebox
