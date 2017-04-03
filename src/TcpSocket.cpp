
#include "TcpSocket.h"

#include <string.h>

namespace beebox
{

typedef struct tcp_write_data 
{
	CTcpSocket* tcpSocket;
	char* buffer;
}TcpWriteData;

static void _onTcpAllocBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
static void _onTcpRead(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf);
static void _onTcpWrite(uv_write_t* writRequst, int status);
static void _onTcpClose(uv_handle_t* handle);

CTcpSocket::CTcpSocket(CLoop* loop)
{
	m_type = CSocket::TCP;

	uv_tcp_t* uvTcp = new uv_tcp_t;
	uv_tcp_init(loop->get(), uvTcp);

	init(uvTcp);
}

CTcpSocket::CTcpSocket(uv_tcp_t* uvTcp)
{
	init(uvTcp);
}

CTcpSocket::~CTcpSocket()
{
	delete m_uvTcp;
}

bool CTcpSocket::bind(int port)
{
	if (port < 0 || port > 65535)
	{
		return false;
	}

	struct sockaddr_in addr;
	uv_ip4_addr("0.0.0.0", port, &addr);
	return uv_tcp_bind(m_uvTcp, (const struct sockaddr*)&addr, 0) != 0;
}

void CTcpSocket::read(char* buffer, int size)
{
	m_runtimeData.readBuffer = buffer;
	m_runtimeData.readBufferSize = size;

	uv_read_start((uv_stream_t*)m_uvTcp, _onTcpAllocBuffer, _onTcpRead);
}

void CTcpSocket::write(const char* buffer, int size)
{
	if (uv_is_writable((uv_stream_t*)m_uvTcp))
	{
		TcpWriteData* writeData = new TcpWriteData;
		writeData->buffer = new char[size];
		memcpy(writeData->buffer, buffer, size);
		writeData->tcpSocket = this;

		uv_buf_t* chuckList = NULL;
		int chuckNum = calculateBuffer(writeData->buffer, size, &chuckList);

		uv_write_t* writeReq = new uv_write_t;
		writeReq->data = writeData;
		uv_write(writeReq, (uv_stream_t*)m_uvTcp, chuckList, chuckNum, _onTcpWrite);

		delete chuckList;
	}
	else
	{
		printf("uv_is_writable false!!!\n");
		close();
	}
}

void CTcpSocket::close()
{
	if (!m_closed)
	{
		uv_close((uv_handle_t*)m_uvTcp, _onTcpClose);
	}
}

void CTcpSocket::init(uv_tcp_t* uvTcp)
{
	m_uvTcp = uvTcp;

	m_closed = false;

	memset(&m_runtimeData, 0, sizeof(m_runtimeData));
	m_runtimeData.TcpSocket = this;
	m_uvTcp->data = (void*)&m_runtimeData;
}

/********************************** libuv callback ************************************/
static void _onTcpAllocBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
	CTcpSocket::TcpRuntimeData* runtimeData = (CTcpSocket::TcpRuntimeData*)handle->data;

	buf->base = runtimeData->readBuffer;
	buf->len = runtimeData->readBufferSize;
}

static void _onTcpRead(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf)
{
	CTcpSocket::TcpRuntimeData* runtimeData = (CTcpSocket::TcpRuntimeData*)client->data;

	if (nread < 0)
	{
		if (nread != UV_EOF)
			fprintf(stderr, "TCP read error: %s\n", uv_strerror(nread));
		runtimeData->TcpSocket->close();
	}
	else if (nread > 0)
	{
		runtimeData->TcpSocket->afterRead(buf->base, nread);
	}
}

static void _onTcpWrite(uv_write_t* writRequst, int status)
{
	bool ret = status ? false : true;

	if (status)
	{
		printf("write error...\n");
	}

	TcpWriteData* writeData = (TcpWriteData*)writRequst->data;

	CTcpSocket* TcpSocket = writeData->tcpSocket;
	if (TcpSocket)
	{
		TcpSocket->afterWrite(ret);
	}

	delete writeData->buffer;
	delete writeData;
	delete writRequst;
}

static void _onTcpClose(uv_handle_t* handle)
{
	CTcpSocket::TcpRuntimeData* runtimeData = (CTcpSocket::TcpRuntimeData*)handle->data;
	runtimeData->TcpSocket->afterClose();
}

} // namespace beebox
