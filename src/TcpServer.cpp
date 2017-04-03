
#include "TcpServer.h"

namespace beebox
{

CTcpServer::CTcpServer(CLoop* loop, int port)
{
	m_loop = loop;

	uv_tcp_init(m_loop->get(), &m_server);

	m_runtimeData.loop = m_loop->get();
	m_runtimeData.tcpServer = this;
	m_server.data = (void*)(&m_runtimeData);

	struct sockaddr_in addr;
	uv_ip4_addr("0.0.0.0", port, &addr);
	uv_tcp_bind(&m_server, (const struct sockaddr*)&addr, 0);

	if (listen() < 0)
	{
		printf("Listen error...");
	}

	m_runByself = false;
}

CTcpServer::~CTcpServer()
{
	recover();
}

int CTcpServer::listen()
{
	return uv_listen((uv_stream_t*)&m_server, 128, CTcpServer::_onConnection);
}

void CTcpServer::recover()
{
	std::vector<CTcpSocket*>::iterator it = m_closedConnectionList.begin();
	while (it != m_closedConnectionList.end())
	{
		if ((*it)->isClosed())
		{
			delete *it;
			it = m_closedConnectionList.erase(it);
		}
		else
		{
			it++;
		}
	}
}

void CTcpServer::afterConnected(CTcpSocket* TcpSocket)
{
	TcpSocket->doClose.connect(this, &CTcpServer::onTcpSocketClose);
	onConnected(TcpSocket);
}

void CTcpServer::onTcpSocketClose(CSocket* TcpSocket)
{
	m_closedConnectionList.push_back((CTcpSocket*)TcpSocket);
}

void CTcpServer::_onConnection(uv_stream_t* server, int status)
{
	printf("onConnection...\n");

	RuntimeData* runtimeData = (RuntimeData*)server->data;
	
	uv_loop_t* loop = runtimeData->loop;
	uv_tcp_t*  client = new uv_tcp_t;
	uv_tcp_init(loop, client);

	if (uv_accept(server, (uv_stream_t*)client) == 0)
	{
		CTcpServer* server = runtimeData->tcpServer;
		if (server)
		{
			CTcpSocket* tcpSocket = new CTcpSocket(client);
			tcpSocket->doClose.connect(server, &CTcpServer::onTcpSocketClose);
			server->onConnected(tcpSocket);

			server->recover();
		}
	}
	else
	{
		uv_close((uv_handle_t*)client, NULL);
	}
}

} // namespace beebox
