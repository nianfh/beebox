
#include "HttpConnection.h"
#include "Date.h"
#include "Utils.h"
#include "File.h"

#include <string.h>

namespace beebox
{

//#define HTTP_DEBUG

// ******************************************************** //
#define PARSER_BUFFER_SIZE 2048
#define HTTP_FILE_BUFFER (1024 * 1024)

static int onMessageBegin(http_parser* parser)
{
#ifdef HTTP_DEBUG
	printf("\n***MESSAGE BEGIN***\n");
#endif
	CHttpConnection* httpConnection = (CHttpConnection*)parser->data;
	httpConnection->clearRequestBody();

	return 0;
}

static int onHeadersComplete(http_parser* parser)
{
#ifdef HTTP_DEBUG
	printf("\n***HEADERS COMPLETE***\n");

	printf("Method = %d\n", parser->method);
	//printf("http_should_keep_alive = %d\n", http_should_keep_alive(parser));
#endif

	CHttpConnection* httpConnection = (CHttpConnection*)parser->data;
	httpConnection->setResponseHeader(
		"Connection",
		http_should_keep_alive(parser) == 0 ? "Keep-Alive" : "Close");

	string method = httpConnection->getRequestMethod();
	string contentType = httpConnection->getRequestHeaderFieldValue("Content-Type");
	if (method == "POST" && contentType.find("multipart/form-data;") != string::npos)
	{
		size_t pos = contentType.find("boundary=") + 9;
		string boundary = contentType.substr(pos);
		httpConnection->enableUploadLogic(boundary);
	}

	return 0;
}

static int onUrl(http_parser* parser, const char* url, size_t length)
{
	CHttpConnection* httpConnection = (CHttpConnection*)parser->data;
#ifdef HTTP_DEBUG
	printf("Url: %.*s\n", (int)length, url);
#endif
	// TODO - use https://github.com/bnoordhuis/uriparser2 instead?
	struct http_parser_url u;
	int result = http_parser_parse_url(url, length, 0, &u);
	if (result)
	{
		fprintf(stderr, "\n\n*** failed to parse URL %s ***\n\n", url);
		return -1;
	}
	else
	{
		if ((u.field_set & (1 << UF_PATH)))
		{
			httpConnection->setRequestPath(std::string(url + u.field_data[UF_PATH].off, u.field_data[UF_PATH].len));
		}

		if ((u.field_set & (1 << UF_QUERY)))
		{
			httpConnection->setRequestQuery(std::string(url + u.field_data[UF_QUERY].off, u.field_data[UF_QUERY].len));
		}

		if (u.field_set == 0)
		{
			fprintf(stderr, "\n\n*** failed to parse PATH in URL %s ***\n\n", url);
			return -1;
		}
	}

	return 0;
}

static int onHeaderField(http_parser* parser, const char* at, size_t length)
{
#ifdef HTTP_DEBUG
	printf("Header field: %.*s\n", (int)length, at);
#endif

	CHttpConnection* httpConnection = (CHttpConnection*)parser->data;
	httpConnection->setRequestLastHeaderFieldName(string(at, length));

	return 0;
}

static int onHeaderValue(http_parser* parser, const char* at, size_t length)
{
#ifdef HTTP_DEBUG
	printf("Header value: %.*s\n", (int)length, at);
#endif

	CHttpConnection* httpConnection = (CHttpConnection*)parser->data;
	httpConnection->setRequestHeaderFieldValue(string(at, length));

	return 0;
}

static int onBody(http_parser* parser, const char* at, size_t length)
{
#ifdef HTTP_DEBUG
	printf("Body(%d): %.*s\n", length, (int)length, at);
#endif

	CHttpConnection* httpConnection = (CHttpConnection*)parser->data;
	if (httpConnection->isUploading())
	{
		httpConnection->writeCacheFile(at, length);
	}
	else
	{
		httpConnection->addRequestBody(string(at, length));
	}

	return 0;
}

static int onMessageComplete(http_parser* parser)
{
#ifdef HTTP_DEBUG
	printf("\n***MESSAGE COMPLETE***\n");
#endif

	CHttpConnection* httpConnection = (CHttpConnection*)parser->data;
	httpConnection->resetResponseHeader();

	if (httpConnection->isUploading())
	{
		httpConnection->uploadEnd();
	}

	httpConnection->doRoute(httpConnection);

	return 0;
}

CHttpConnection::CHttpConnection(CSocket* socket)
{
	m_socket = socket;
	m_socket->doRead.connect(this, &CHttpConnection::onRead);
	m_socket->doWrite.connect(this, &CHttpConnection::onWrite);
	m_socket->doClose.connect(this, &CHttpConnection::onClosed);

	/************** Request **************/
	m_httpSettings.on_url = onUrl;
	// notification callbacks
	m_httpSettings.on_message_begin = onMessageBegin;
	m_httpSettings.on_headers_complete = onHeadersComplete;
	m_httpSettings.on_message_complete = onMessageComplete;
	// data callbacks
	m_httpSettings.on_header_field = onHeaderField;
	m_httpSettings.on_header_value = onHeaderValue;
	m_httpSettings.on_body = onBody;

	m_httpParser.data = (void*)this;

	m_recvBuffer = new char[PARSER_BUFFER_SIZE];
	memset(m_recvBuffer, 0, PARSER_BUFFER_SIZE);

	m_haveResponded = false;

	m_uploading = false;
	m_uploadingFile = NULL;

	http_parser_init(&m_httpParser, HTTP_REQUEST);

	/************** Response **************/
	resetResponseHeader();
}

CHttpConnection::~CHttpConnection()
{
	delete m_recvBuffer;
}

void CHttpConnection::start()
{
	m_socket->read(m_recvBuffer, PARSER_BUFFER_SIZE);
}

void CHttpConnection::stop()
{
	m_socket->close();
}

std::string CHttpConnection::getRequestMethod()
{
	return http_method_str((enum http_method)m_httpParser.method);
}

std::string CHttpConnection::getRequestPath()
{
	return m_path;
}

std::string CHttpConnection::getRequestQuery()
{
	return m_query;
}

std::string CHttpConnection::getRequestBody()
{
	return m_body;
}

std::string CHttpConnection::getRequestHeaderFieldValue(string fieldName)
{
	map<string, string>::iterator it = m_headerMap.find(fieldName);
	if (it != m_headerMap.end())
	{
		return it->second;
	}

	return "";
}


void CHttpConnection::setResponseStatus(string status)
{
	m_status = status;
}

void CHttpConnection::setResponseHeader(string fieldName, string fieldValue)
{
	m_resposeHeaderMap[fieldName] = fieldValue;
}

bool CHttpConnection::isResponseKeepAlive()
{
	return m_resposeHeaderMap["Connection"] != "Close";
}

void CHttpConnection::resetResponseHeader()
{
	m_resposeHeaderMap.clear();

	m_status = HTTP_STATUS_200;
	setResponseHeader("Connection", "Keep-Alive");
	setResponseHeader("Server", "Beebox Server 1.0");

	m_haveResponded = false;
}

void CHttpConnection::sendResponseSimple(string body)
{
	sendResponseSimple((char*)body.c_str(), body.size());
}

void CHttpConnection::sendResponseSimple(char* bodyBuffer, int bodySize)
{
	// build http header
	string temp;
	CUtils::stringFormat(temp, "%d", bodySize);
	CDate date(CDate::GMT);

	setResponseHeader("Content-Length", temp);
	setResponseHeader("Accept-Ranges", "none");
	setResponseHeader("Date", date.toStringForHttp());

	sendResponse(bodyBuffer, bodySize);
}

void CHttpConnection::sendResponse(char* bodyBuffer, int bodySize, bool withEnd/*=true*/)
{
	string header = "HTTP/1.1 ";
	header += m_status + CRLF;

	map<string, string>::iterator it = m_resposeHeaderMap.begin();
	while (it != m_resposeHeaderMap.end())
	{
		header += it->first + ": " + it->second + CRLF;
		++it;
	}
	header += CRLF;

	// build send buffer
	int length = header.size() + bodySize;
	if (withEnd)
	{
		length += 2;
	}
	char* buffer = new char[length];
	memcpy(buffer, header.c_str(), header.size());
	int copyLength = header.size();

	if (bodyBuffer && bodySize > 0)
	{
		memcpy(buffer + copyLength, bodyBuffer, bodySize);
		copyLength += bodySize;
	}

	if (withEnd)
	{
		memcpy(buffer + copyLength, CRLF, 2);
	}

	// send
	m_socket->write(buffer, length);

	delete buffer;
}

void CHttpConnection::sendResponseDirct(string content)
{
	m_socket->write(content.c_str(), content.size());
}

void CHttpConnection::sendResponseDirct(char* buffer, int size)
{
	m_socket->write(buffer, size);
}

bool CHttpConnection::haveResponded()
{
	return m_haveResponded;
}

bool CHttpConnection::sendFile(string filePath)
{
	bool ret = false;

	CFile file(filePath);
	if (file.isOpened())
	{
		int size = file.getSize();
		int readSize = 0;
		string sizeStr;
		CUtils::stringFormat(sizeStr, "%d", size);
		char* buffer = new char[HTTP_FILE_BUFFER];
		int len = file.read(buffer, HTTP_FILE_BUFFER);
		if (len > 0)
		{
			string disposition = "attachment; filename=" + file.getFullName();

			setResponseStatus(HTTP_STATUS_200);
			setResponseHeader("Connection", "Close");
			setResponseHeader("Content-Length", sizeStr);
			setResponseHeader("Content-Type", "application/octet-stream");
			setResponseHeader("Content-Description", "File Transfer");
			setResponseHeader("Content-Disposition", disposition);
			setResponseHeader("Cache-Control", "Public");
			setResponseHeader("Access-Control-Allow-Origin", "*");
			setResponseHeader("Date", CDate(CDate::GMT).toString());
			sendResponse(buffer, len, false);

			readSize += len;
			while (readSize < size)
			{
				len = file.read(buffer, HTTP_FILE_BUFFER);
				sendResponseDirct(buffer, len);
				readSize += len;
			}

			ret = true;
		}

		delete buffer;
	}

	return ret;
}

void CHttpConnection::setUploadingCacheFileDir(string filePath)
{
	m_uploadingFileDir = filePath;
}

std::string CHttpConnection::getUploadingCacheFilePath()
{
	return m_uploadingFilePath;
}

std::string CHttpConnection::getContentType(string fileExtension)
{
	if (fileExtension.empty())
	{
		fileExtension = "html";
	}

	string contentType = "text/";
	if (fileExtension == "jpg" || fileExtension == "png" || fileExtension == "gif")
	{
		if (fileExtension == "jpg")
		{
			fileExtension = "jpeg";
		}

		contentType = "image/";
	}

	return contentType + fileExtension;
}

void CHttpConnection::onRead(int length)
{
	bool isOK = false;
	if (length > 10)
	{
		isOK = parseRequest(m_recvBuffer, length);
		memset(m_recvBuffer, 0, length);
	}

	if (!isOK)
	{
		if (m_socket->getType() == CSocket::TCP)
		{
			stop();
		}
	}
}

void CHttpConnection::onWrite(bool isOK)
{
	if (!isOK && m_socket->getType() == CSocket::TCP)
	{
		stop();
	}

	m_haveResponded = true;
}

void CHttpConnection::onClosed(CSocket* socket)
{
	doDisconnect(this);
}

void CHttpConnection::setRequestPath(string path)
{
	m_path = path;
}

void CHttpConnection::addRequestBody(string body)
{
	m_body += body;
}

void CHttpConnection::clearRequestBody()
{
	m_body.clear();
}

void CHttpConnection::setUploading(string fileName)
{
	m_uploadingFilePath = fileName;
}

void CHttpConnection::setRequestQuery(string query)
{
	CUtils::stringReplace(query, "%20", " ");
	m_query = query;
}

void CHttpConnection::setRequestLastHeaderFieldName(string fieldName)
{
	m_lastHeaderField = fieldName;
}

void CHttpConnection::setRequestHeaderFieldValue(string fieldValue)
{
	m_headerMap[m_lastHeaderField] = fieldValue;
}

bool CHttpConnection::isUploading()
{
	return m_boundary.size() > 0;
}

void CHttpConnection::enableUploadLogic(string boundary)
{
	m_boundary = boundary;

	string filePath;
	CUtils::stringFormat(filePath, "%s/0x%p_%s", m_uploadingFileDir.c_str(), this, CDate().toString("%Y%m%d%H%M%S").c_str());

	m_uploadingFile = new CFile;
	if (m_uploadingFile->open(filePath, CFile::READ_WRITE | CFile::CREATE))
	{
		m_uploading = true;
	}
}

void CHttpConnection::writeCacheFile(const char* buffer, int length)
{
	if (m_uploading && m_uploadingFile)
	{
		m_uploadingFile->write((void*)buffer, length);
	}
}

void CHttpConnection::uploadEnd()
{
	m_uploadingFile->flush();
	m_uploadingFile->close();

	m_uploadingFile->open();
	int size = m_uploadingFile->getSize();
	int expectedSize = size > 1024 ? 1024 : size;
	char* buffer = new char[HTTP_FILE_BUFFER];

	// get file infomation
	// get the top 1024 bytes, should be enough
	int len = m_uploadingFile->read(buffer, expectedSize);
	string sourceStr(buffer, len);
	string beginFlagStr = "--" + m_boundary;
	string endFlagStr = m_boundary + "--";
	string info, tempStr;

	size_t beginFlagPos = sourceStr.find(beginFlagStr);
	size_t infoPos = beginFlagPos + beginFlagStr.size() + 2;
	size_t infoEndPos = sourceStr.find(string(CRLF) + CRLF);
	size_t contentPos = infoEndPos + 4;
	
	// get ContentDispostion
	size_t pos = sourceStr.find(CRLF, infoPos);
	tempStr = sourceStr.substr(infoPos, pos - infoPos);
	pos = tempStr.find(": ");
	string infoContentDispostionValue = tempStr.substr(pos + 2);

	// get filename
	vector<string> stringArray;
	CUtils::stringSplit(infoContentDispostionValue, stringArray, "; ");
	string filename = stringArray[2];
	pos = filename.find('=') + 2;
	filename = filename.substr(pos, filename.size()-1 - pos);

	size_t endFlagPos = (size - 1) - (endFlagStr.size() + 2);
	size_t contentEndPos = endFlagPos - 1;
	
	int actualSize = contentEndPos - contentPos;
	m_uploadingFile->seek(contentPos , CFile::BEGIN);
	int shouldReadSize = HTTP_FILE_BUFFER;
	CFile file; // last file
	m_uploadingFilePath = m_uploadingFileDir + filename;
	file.open(m_uploadingFilePath, CFile::READ_WRITE | CFile::CREATE);
	int readSize = 0;
	while (readSize < actualSize)
	{
		shouldReadSize = actualSize - readSize;
		if (shouldReadSize > HTTP_FILE_BUFFER)
		{
			shouldReadSize = HTTP_FILE_BUFFER;
		}

		len = m_uploadingFile->read(buffer, shouldReadSize);
		file.write(buffer, len);

		readSize += len;
	}

	m_uploadingFile->close();
	// remove raw data file
	CFile::remove(m_uploadingFile->getFullPath());
	delete m_uploadingFile;
	m_uploadingFile = NULL;

	file.flush();
	file.close();
}

bool CHttpConnection::parseRequest(char* buffer, int length)
{
	bool isOK = false;

	ssize_t parsed = http_parser_execute(&m_httpParser, &m_httpSettings, buffer, length);
	if (parsed < length)
	{
		printf("parse error[%d]:%s -> %s\n", m_httpParser.http_errno,
			http_errno_name(HTTP_PARSER_ERRNO(&m_httpParser)),
			http_errno_description(HTTP_PARSER_ERRNO(&m_httpParser)));
		printf("%.*s\n", length, buffer);

		// UDP, reset http parser.
		// if socket is TCP, this http connection will close by next step.
		if (m_socket->getType() == CSocket::UDP)
		{
			http_parser_init(&m_httpParser, HTTP_REQUEST);
		}
	}
	else
	{
		isOK = true;
	}

	return isOK;
}

} // namespace beebox
