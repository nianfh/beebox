
#ifndef __SOCKET_H__
#define __SOCKET_H__

#include "Loop.h"
#include "sigslot.h"

namespace beebox
{

#define CHUNK_SIZE (4 * 1024)

class CSocket : public sigslot::has_slots<>
{
public:
	enum socket_type
	{
		TCP,
		UDP
	};

	CSocket();
	virtual ~CSocket();

	virtual bool bind(int port) = 0;
	virtual void read(char* buffer, int size) = 0;
	virtual void write(const char* buffer, int size) = 0;
	virtual void close() = 0;

	int getType();
	bool isClosed();

	sigslot::signal1<CSocket*> doClose;
	sigslot::signal1<int> doRead;
	sigslot::signal1<bool> doWrite;

	// called by libuv's callback
	virtual void afterRead(char* buffer, int size);
	virtual void afterWrite(bool isOK);
	virtual void afterClose();

protected:
	int calculateBuffer(const char* buffer, int size, uv_buf_t** bufList);

	int m_type;
	int m_closed;
};

}

#endif // __SOCKET_H__
