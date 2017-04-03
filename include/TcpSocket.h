
#ifndef __TCP_SOCKET_H__
#define __TCP_SOCKET_H__

#include "Socket.h"

namespace beebox
{

class CTcpSocket : public CSocket
{
public:
	// used as client
	CTcpSocket(CLoop* loop);
	// used as server, CTcpServer will use it
	// when new TCP connection is established
	CTcpSocket(uv_tcp_t* uvTcp);
	~CTcpSocket();

	bool bind(int port);
	void read(char* buffer, int size);
	void write(const char* buffer, int size);
	void close();

	// emit when TCP connection is established
	sigslot::signal1<bool> doConnected;

	// internal use
	typedef struct _runtime_data_
	{
		CTcpSocket* TcpSocket;

		char* readBuffer;
		int   readBufferSize;
	}TcpRuntimeData;

private:
	void init(uv_tcp_t* uvTcp);

private:
	uv_tcp_t*  m_uvTcp;

	TcpRuntimeData m_runtimeData;
};

}

#endif // __TCP_SOCKET_H__
