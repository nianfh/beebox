
#ifndef __UDP_SOCKET_H__
#define __UDP_SOCKET_H__

#include "Socket.h"
#include <string>

namespace beebox
{

class CUdpSocket : public CSocket
{
public:
	CUdpSocket(CLoop* loop);
	~CUdpSocket();

	bool bind(int port);
	void read(char* buffer, int size);
	void write(const char* buffer, int size);
	void close();

	bool joinMulticastGroup(std::string groupIP);
	bool setMulticastLoop(bool enable);
	bool setMulticastTTL(int timeToLive);
	void setTarget(std::string ip, int port);

	typedef struct _udp_runtime_data_
	{
		char* readBuffer;
		int   readBufferSize;
		CUdpSocket*  udp;
	}UdpRuntimeData;

private:
	uv_udp_t  m_uvUdp;

	std::string m_ip;
	int m_port;

	UdpRuntimeData m_runtimeData;
};

}

#endif // __UDP_SOCKET_H__
