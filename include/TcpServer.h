
#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include <vector>

#include "TcpSocket.h"

namespace beebox
{

class CTcpServer : public sigslot::has_slots<>
{
public:
	CTcpServer(CLoop* loop, int port);
	~CTcpServer();

	void recover();

	void afterConnected(CTcpSocket* TcpSocket);
	void onTcpSocketClose(CSocket* TcpSocket);

	sigslot::signal1<CTcpSocket*> onConnected;

private:
	int listen();

	static void _onConnection(uv_stream_t* server, int status);

private:
	CLoop* m_loop;
	uv_tcp_t  m_server;

	bool m_runByself;

	typedef struct _exdata
	{
		uv_loop_t* loop;
		CTcpServer*  tcpServer;
	}RuntimeData;
	RuntimeData m_runtimeData;

	std::vector<CTcpSocket*> m_closedConnectionList;
};

}

#endif // __TCP_SERVER_H__
