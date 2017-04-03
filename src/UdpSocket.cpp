
#include "UdpSocket.h"

#include <string.h>

namespace beebox
{

#define CHUNK_SIZE (4 * 1024)

typedef struct udp_write_data
{
	CUdpSocket* udpSocket;
	char* buffer;
}UdpWriteData;

static void _onUdpAllocBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
static void _onUdpRead(uv_udp_t* handle,
	ssize_t nread,
	const uv_buf_t* buf,
	const struct sockaddr* addr,
	unsigned flags);
static void _onUdpWrite(uv_udp_send_t* req, int status);
static void _onUdpClose(uv_handle_t* handle);

CUdpSocket::CUdpSocket(CLoop* loop)
{
	m_type = CSocket::UDP;

	m_runtimeData.udp = this;
	m_uvUdp.data = (void*)(&m_runtimeData);

	uv_udp_init(loop->get(), &m_uvUdp);
	
	m_port = 0;
}

CUdpSocket::~CUdpSocket()
{

}

bool CUdpSocket::bind(int port)
{
	if (port < 0 || port > 65535)
	{
		return false;
	}

	struct sockaddr_in addr;
	uv_ip4_addr("0.0.0.0", port, &addr);
	return uv_udp_bind(&m_uvUdp, (const struct sockaddr*) &addr, 0) != 0;
}

void CUdpSocket::read(char* buffer, int size)
{
	m_runtimeData.readBuffer = buffer;
	m_runtimeData.readBufferSize = size;

	uv_udp_recv_start(&m_uvUdp, _onUdpAllocBuffer, _onUdpRead);
}

void CUdpSocket::write(const char* buffer, int size)
{
	UdpWriteData* writeData = new UdpWriteData;
	writeData->buffer = new char[size];
	memcpy(writeData->buffer, buffer, size);
	writeData->udpSocket = this;

	uv_buf_t* chuckList = NULL;
	int chuckNum = calculateBuffer(writeData->buffer, size, &chuckList);

	uv_udp_send_t* sendReq = new uv_udp_send_t;
	sendReq->data = writeData;
	
	struct sockaddr_in addr;
	uv_ip4_addr(m_ip.c_str(), m_port, &addr);
	uv_udp_send(sendReq, &m_uvUdp,
		chuckList, chuckNum,
		(const struct sockaddr*) &addr,
		_onUdpWrite);

	//printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ udp write chuck[%p] size=%d, req=%p, handle=%p\n", chuckList, chuckNum, sendReq, &m_uvUdp);
	delete chuckList;
}

void CUdpSocket::close()
{
	uv_close((uv_handle_t*)&m_uvUdp, _onUdpClose);
}

bool CUdpSocket::joinMulticastGroup(std::string groupIP)
{
	/* join the multicast channel */
	int r = uv_udp_set_membership(&m_uvUdp, groupIP.c_str(), NULL, UV_JOIN_GROUP);
	if (r == UV_ENODEV)
	{
		printf("No multicast support.");
	}

	return r == 0 ? false : true;
}

bool CUdpSocket::setMulticastLoop(bool enable)
{
	return uv_udp_set_multicast_loop(&m_uvUdp, enable ? 1 : 0) != 0;
}

bool CUdpSocket::setMulticastTTL(int timeToLive)
{
	return  uv_udp_set_multicast_ttl(&m_uvUdp, timeToLive) != 0;
}

void CUdpSocket::setTarget(std::string ip, int port)
{
	m_ip = ip;
	m_port = port;
}

/********************************** libuv callback ************************************/
static void _onUdpAllocBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
	CUdpSocket::UdpRuntimeData* runtimeData = (CUdpSocket::UdpRuntimeData*)handle->data;

	buf->base = runtimeData->readBuffer;
	buf->len = runtimeData->readBufferSize;
}

static void _onUdpRead(uv_udp_t* handle,
	ssize_t nread,
	const uv_buf_t* buf,
	const struct sockaddr* addr,
	unsigned flags)
{
	CUdpSocket::UdpRuntimeData* runtimeData = (CUdpSocket::UdpRuntimeData*)handle->data;

	if (nread < 0)
	{
		if (nread != UV_EOF)
			fprintf(stderr, "TCP read error: %s\n", uv_strerror(nread));
	}
	else if (nread > 0)
	{
		runtimeData->udp->afterRead(buf->base, nread);
	}
}

static void _onUdpWrite(uv_udp_send_t* req, int status)
{
	bool ret = status ? false : true;

	if (!ret)
	{
		printf("write error[%s]...\n", uv_strerror(status));
	}

	UdpWriteData* writeData = (UdpWriteData*)req->data;

	CUdpSocket* udp = writeData->udpSocket;
	if (udp)
	{
		udp->afterWrite(ret);
	}

	delete writeData->buffer;
	delete writeData;
	delete req;
}

static void _onUdpClose(uv_handle_t* handle)
{
	CUdpSocket::UdpRuntimeData* runtimeData = (CUdpSocket::UdpRuntimeData*)handle->data;
	runtimeData->udp->afterClose();
}

} // namespace beebox
