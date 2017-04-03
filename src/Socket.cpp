
#include "Socket.h"

namespace beebox
{

CSocket::CSocket()
{
	m_type = TCP;
	m_closed = false;
}

CSocket::~CSocket()
{

}

int CSocket::getType()
{
	return m_type;
}

bool CSocket::isClosed()
{
	return m_closed;
}

void CSocket::afterRead(char* buffer, int size)
{
	doRead(size);
}

void CSocket::afterWrite(bool isOK)
{
	doWrite(isOK);
}

void CSocket::afterClose()
{
	m_closed = true;
	doClose(this);
}

int CSocket::calculateBuffer(const char* buffer, int size, uv_buf_t** bufList)
{
	int chuckNum = size / CHUNK_SIZE;
	if (chuckNum * CHUNK_SIZE < size)
	{
		++chuckNum;
	}

	uv_buf_t* chuckList = new uv_buf_t[chuckNum];
	int writedSize = 0;
	for (int i = 0; i < chuckNum; ++i)
	{
		int currentChuckSize = CHUNK_SIZE;
		int remainSize = size - writedSize;
		if (remainSize < CHUNK_SIZE)
		{
			currentChuckSize = remainSize;
		}

		chuckList[i] = uv_buf_init((char*)buffer + writedSize, currentChuckSize);

		writedSize += currentChuckSize;
	}

	*bufList = chuckList;
	return chuckNum;
}

} // namespace beebox
